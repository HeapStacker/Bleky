local projectName = tostring(os.getenv("PIXI_PROJECT_NAME"))
local projectVersion = os.getenv("PIXI_PROJECT_VERSION")

set_project(projectName)
set_version(projectVersion)
set_languages("c++23")

add_rules("mode.debug", "mode.release")
set_warnings("all", "error")

add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })

if is_plat("windows", "mingw") and not is_config("build_android") then
    set_toolchains("msvc")
    add_defines("WIN32_LEAN_AND_MEAN", "NOMINMAX")
    add_cxflags("/utf-8", "/Zc:__cplusplus")
end

package("logger")
    add_urls("https://github.com/HeapStacker/Logger.git")
    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)
package_end()

package("vault")
    add_urls("https://github.com/HeapStacker/Vault.git")
    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)
package_end()

add_requires(
    "logger",
    "vault"
)

target("Bleky")
    set_kind("static")
    add_files("src/**.cpp")
    add_includedirs("include", {public = true})
    add_includedirs("private")
    add_packages("logger", "vault", {public = true})
target_end()

task("docs")
    on_run(function ()
        os.exec("mkdocs serve")
    end)