/*
 *
 * Copyright 2022 Apple Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "view/window.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#pragma mark - AppDelegate
#pragma region AppDelegate {

MyAppDelegate::~MyAppDelegate()
{
	_pMtkView->release();
	_pWindow->release();
	_pDevice->release();
	delete _pViewDelegate;
}

NS::Menu* MyAppDelegate::createMenuBar()
{
	using NS::StringEncoding::UTF8StringEncoding;

	NS::Menu* pMainMenu = NS::Menu::alloc()->init();
	NS::MenuItem* pAppMenuItem = NS::MenuItem::alloc()->init();
	NS::Menu* pAppMenu = NS::Menu::alloc()->init( NS::String::string( "Appname", UTF8StringEncoding ) );

	NS::String* appName = NS::RunningApplication::currentApplication()->localizedName();
	NS::String* quitItemName = NS::String::string( "Quit ", UTF8StringEncoding )->stringByAppendingString( appName );
	SEL quitCb = NS::MenuItem::registerActionCallback( "appQuit", [](void*,SEL,const NS::Object* pSender){
		auto pApp = NS::Application::sharedApplication();
		pApp->terminate( pSender );
	} );

	NS::MenuItem* pAppQuitItem = pAppMenu->addItem( quitItemName, quitCb, NS::String::string( "q", UTF8StringEncoding ) );
	pAppQuitItem->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );
	pAppMenuItem->setSubmenu( pAppMenu );

	NS::MenuItem* pWindowMenuItem = NS::MenuItem::alloc()->init();
	NS::Menu* pWindowMenu = NS::Menu::alloc()->init( NS::String::string( "Window", UTF8StringEncoding ) );

	SEL closeWindowCb = NS::MenuItem::registerActionCallback( "windowClose", [](void*, SEL, const NS::Object*){
		auto pApp = NS::Application::sharedApplication();
		pApp->windows()->object< NS::Window >(0)->close();
	} );
	NS::MenuItem* pCloseWindowItem = pWindowMenu->addItem( NS::String::string( "Close Window", UTF8StringEncoding ), closeWindowCb, NS::String::string( "w", UTF8StringEncoding ) );
	pCloseWindowItem->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );

	pWindowMenuItem->setSubmenu( pWindowMenu );

	pMainMenu->addItem( pAppMenuItem );
	pMainMenu->addItem( pWindowMenuItem );

	pAppMenuItem->release();
	pWindowMenuItem->release();
	pAppMenu->release();
	pWindowMenu->release();

	return pMainMenu->autorelease();
}

void MyAppDelegate::applicationWillFinishLaunching( NS::Notification* pNotification )
{
	NS::Menu* pMenu = createMenuBar();
	NS::Application* pApp = reinterpret_cast< NS::Application* >( pNotification->object() );
	pApp->setMainMenu( pMenu );
	pApp->setActivationPolicy( NS::ActivationPolicy::ActivationPolicyRegular );
}

void MyAppDelegate::applicationDidFinishLaunching( NS::Notification* pNotification )
{
	CGRect frame = (CGRect){ {100.0, 100.0}, {512.0, 512.0} };

	_pWindow = NS::Window::alloc()->init(
			frame,
			NS::WindowStyleMaskClosable|NS::WindowStyleMaskTitled,
			NS::BackingStoreBuffered,
			false );

	_pDevice = MTL::CreateSystemDefaultDevice();

	_pMtkView = MTK::View::alloc()->init( frame, _pDevice );
	_pMtkView->setColorPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
	_pMtkView->setClearColor( MTL::ClearColor::Make( 0.30, 0.31, 0.34, 1.0 ) );

	_pViewDelegate = new MyMTKViewDelegate( _pDevice );
	_pMtkView->setDelegate( _pViewDelegate );

	_pWindow->setContentView( _pMtkView );
	_pWindow->setTitle( NS::String::string( "Sample", NS::StringEncoding::UTF8StringEncoding ) );

	_pWindow->makeKeyAndOrderFront( nullptr );

	auto* pApp = reinterpret_cast< NS::Application* >( pNotification->object() );
	pApp->activateIgnoringOtherApps( true );

	{
		std::lock_guard<std::mutex> lock(mtx);
		finished = true;
	}
	cv.notify_one();
}

bool MyAppDelegate::applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender )
{
	return true;
}

void MyAppDelegate::setViewDelegate(MyMTKViewDelegate *viewDelegate) {
	_pViewDelegate=viewDelegate;
}

#pragma endregion AppDelegate }


#pragma mark - ViewDelegate
#pragma region ViewDelegate {

MyMTKViewDelegate::MyMTKViewDelegate( MTL::Device* pDevice )
		: MTK::ViewDelegate()
		, _pRenderer( std::make_unique<Renderer<Square>>( pDevice ) )
{
}

void MyMTKViewDelegate::drawInMTKView( MTK::View* pView )
{
	_pRenderer->draw( pView );
}

#pragma endregion ViewDelegate }

