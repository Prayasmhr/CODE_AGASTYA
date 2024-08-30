vulkan_lib = -IC:/VulkanSDK/1.3.250.1/Include -LC:/VulkanSDK/1.3.250.1/Lib
teja_ui = teja_ui/**.c
tjlang = tjlang/**.c
CFLAGS = -Wall -Werror -Wpedantic -Wno-unused-function -Wno-switch -D_CRT_SECURE_NO_WARNINGS -D_MT

all:
	clang $(teja_ui) $(tjlang) agy_app/**.c -o code_agy.exe -Ivendor -Lvendor -lray_renderer -lSDL2 -lSDL2main -lheart_lib $(CFLAGS)

_debug:
	clang $(teja_ui) $(tjlang) $(CFLAGS) agy_app/**.c -g -o debug_agy.exe -Ivendor -Lvendor -lray_renderer -lSDL2 -lSDL2main -lheart_lib	

build_ray_renderer:
	clang ray_renderer/**.c -o ray_renderer.dll -Ivendor -Lvendor -shared $(CFLAGS) -D_RAY_RENDERER_BUILD -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lheart_lib
	rm *.exp
	mv ray_renderer.lib vendor

build_photon8_renderer:
	clang photon8_renderer/**.c -o photon8_renderer.dll -shared $(CFLAGS) $(vulkan_lib) -lvulkan-1 -lheart_lib
	mv *.exp
	mv *.lib vendor
