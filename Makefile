VENDOR = -Ivendor -Lvendor
vulkan_lib = -IC:/VulkanSDK/1.3.250.1/Include -LC:/VulkanSDK/1.3.250.1/Lib
heart_lib = heart_lib/**.c
teja_ui = teja_ui/**.c
tjlang = tjlang/**.c
CFLAGS = -fsanitize=address -fsanitize=undefined -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wall -Werror -Wpedantic -Wno-unused-function -Wno-switch -D_CRT_SECURE_NO_WARNINGS

all:
	cp teja_ui ../code_agastya_backup/ 
	cp tjlang ../code_agastya_backup/ 
	cp heart_lib ../code_agastya_backup/ 
	cp agy_app ../code_agastya_backup/ 
	clang $(heart_lib) $(teja_ui) $(tjlang) agy_app/**.c -o code_agy.exe $(VENDOR) -lray_renderer -lSDL2 -lSDL2main $(CFLAGS)

_debug:
	clang $(heart_lib) $(teja_ui) $(tjlang) $(CFLAGS) agy_app/**.c -g -o debug_agy.exe $(VENDOR) -lray_renderer -lSDL2 -lSDL2main	

build_ray_renderer:
	clang ray_renderer/**.c $(heart_lib) -o ray_renderer.dll $(VENDOR) -shared $(CFLAGS) -Wpedantic -D_RAY_RENDERER_BUILD -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf 
	rm *.exp
	mv *.lib vendor

build_photon8_renderer:
	clang photon8_renderer/**.c $(heart_lib) -o photon8_renderer.dll -shared $(CFLAGS) $(vulkan_lib) -lvulkan-1
	mv *.exp
	mv *.lib vendor
