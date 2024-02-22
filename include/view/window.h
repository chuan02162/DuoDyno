//
// Created by 韩昱川 on 2/6/24.
//

#ifndef BOX2D_LITE_WINDOW_H
#define BOX2D_LITE_WINDOW_H

#endif //BOX2D_LITE_WINDOW_H

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>
#include "view/renderer.h"

class MyMTKViewDelegate : public MTK::ViewDelegate
{
public:
	MyMTKViewDelegate( MTL::Device* pDevice );
	~MyMTKViewDelegate() override;
	void drawInMTKView( MTK::View* pView ) override;

private:
	Renderer<Shape>* _pRenderer;
};

class MyAppDelegate : public NS::ApplicationDelegate
{
public:
	~MyAppDelegate();

	NS::Menu* createMenuBar();

	virtual void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
	virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
	virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

private:
	NS::Window* _pWindow;
	MTK::View* _pMtkView;
	MTL::Device* _pDevice;
	MyMTKViewDelegate* _pViewDelegate = nullptr;
};
