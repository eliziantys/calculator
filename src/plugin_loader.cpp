#include "plugin_loader.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#  define NOMINMAX
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

struct PluginRegistry::Library {
#ifdef _WIN32
    HMODULE handle = nullptr;
    ~Library() { if (handle) FreeLibrary(handle); }
#else
    void* handle = nullptr;
    ~Library() { if (handle) dlclose(handle); }
#endif
    Library() = default;
    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;
};

namespace {
bool hasLibraryExtension(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
#ifdef _WIN32
    return extension == ".dll";
#elif __APPLE__
    return extension == ".dylib" || extension == ".so";
#else
    return extension == ".so";
#endif
}
}

PluginRegistry::PluginRegistry() = default;
PluginRegistry::~PluginRegistry() = default;

void PluginRegistry::loadFrom(const std::string& directory) {
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!fs::exists(directory, ec) || ec || !fs::is_directory(directory, ec) || ec) {
        std::cout << "Папка плагинов не найдена: " << directory << '\n';
        return;
    }

    std::vector<fs::path> files;
    fs::directory_iterator it(directory, ec), end;
    if (ec) {
        std::cerr << "Не удалось прочитать папку плагинов: " << ec.message() << '\n';
        return;
    }
    for (; it != end; it.increment(ec)) {
        if (ec) { std::cerr << "Ошибка чтения папки плагинов: " << ec.message() << '\n'; ec.clear(); continue; }
        std::error_code typeError;
        if (it->is_regular_file(typeError) && !typeError && hasLibraryExtension(it->path()))
            files.push_back(it->path());
    }
    std::sort(files.begin(), files.end());

    for (const auto& file : files) {
        auto library = std::make_unique<Library>();
#ifdef _WIN32
        library->handle = LoadLibraryW(file.wstring().c_str());
        if (!library->handle) {
            std::cerr << "Пропуск плагина " << file.filename().string() << ": не удалось загрузить DLL\n";
            continue;
        }
        auto init = reinterpret_cast<CalculatorPluginInit>(GetProcAddress(library->handle, "calculator_plugin_init"));
#else
        library->handle = dlopen(file.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!library->handle) {
            const char* reason = dlerror();
            std::cerr << "Пропуск плагина " << file.filename().string() << ": " << (reason ? reason : "ошибка загрузки") << '\n';
            continue;
        }
        dlerror();
        auto init = reinterpret_cast<CalculatorPluginInit>(dlsym(library->handle, "calculator_plugin_init"));
        const char* symbolError = dlerror();
        if (symbolError) init = nullptr;
#endif
        if (!init) {
            std::cerr << "Пропуск плагина " << file.filename().string() << ": отсутствует calculator_plugin_init\n";
            continue;
        }

        try {
            const CalculatorPlugin* plugin = init();
            if (!plugin || plugin->abi_version != CALCULATOR_PLUGIN_ABI_VERSION ||
                !plugin->name || !plugin->function) {
                std::cerr << "Пропуск плагина " << file.filename().string() << ": неверный дескриптор или версия ABI\n";
                continue;
            }
            std::string name(plugin->name);
            if (name.empty() || !std::isalpha(static_cast<unsigned char>(name.front())) ||
                !std::all_of(name.begin(), name.end(), [](unsigned char c) { return std::isalnum(c) || c == '_'; })) {
                std::cerr << "Пропуск плагина " << file.filename().string() << ": некорректное имя функции\n";
                continue;
            }
            if (functions_.count(name)) {
                std::cerr << "Пропуск плагина " << file.filename().string() << ": функция " << name << " уже зарегистрирована\n";
                continue;
            }
            functions_.emplace(name, plugin->function);
            libraries_.push_back(std::move(library));
            std::cout << "Загружена функция: " << name << '\n';
        } catch (const std::exception& ex) {
            std::cerr << "Пропуск плагина " << file.filename().string() << ": ошибка инициализации: " << ex.what() << '\n';
        } catch (...) {
            std::cerr << "Пропуск плагина " << file.filename().string() << ": неизвестная ошибка инициализации\n";
        }
    }
    if (functions_.empty()) std::cout << "Не загружено ни одной функции-плагина. Доступны только +, -, *, /, ^ и скобки.\n";
}

double PluginRegistry::call(const std::string& name, double argument) const {
    auto found = functions_.find(name);
    if (found == functions_.end()) throw std::runtime_error("Неизвестная функция: " + name);
    return found->second(argument);
}

bool PluginRegistry::contains(const std::string& name) const { return functions_.count(name) != 0; }
std::size_t PluginRegistry::size() const noexcept { return functions_.size(); }
