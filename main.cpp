#include <iostream>
#include "view/window.h"
int main(){
	using simd::float4x4;
	using simd::float4;

	NS::AutoreleasePool* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

	MyAppDelegate del;

	NS::Application* pSharedApplication = NS::Application::sharedApplication();
	pSharedApplication->setDelegate( &del );
	pSharedApplication->run();

	pAutoreleasePool->release();

	return 0;
}