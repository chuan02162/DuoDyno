//
// Created by 韩昱川 on 2/6/24.
//

#ifndef BOX2D_LITE_WINDOW_H
#define BOX2D_LITE_WINDOW_H

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>
#include <mutex>
#include <condition_variable>

#include "view/renderer.h"

class MyMTKViewDelegate : public MTK::ViewDelegate
{
public:
	explicit MyMTKViewDelegate( MTL::Device* pDevice );
	void drawInMTKView( MTK::View* pView ) override;

private:
	std::unique_ptr<Renderer<Square>> _pRenderer;
};

class MyAppDelegate : public NS::ApplicationDelegate
{
public:
	~MyAppDelegate() override;

	NS::Menu* createMenuBar();
	void setViewDelegate(MyMTKViewDelegate *viewDelegate);

	virtual void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
	virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
	virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

	std::mutex mtx;
	std::condition_variable cv;
	bool finished;

private:
	NS::Window* _pWindow;
	MTK::View* _pMtkView;
	MTL::Device* _pDevice;
	MyMTKViewDelegate* _pViewDelegate = nullptr;

};


#endif //BOX2D_LITE_WINDOW_H