//#include "view/renderer.h"
//#pragma mark - Renderer
//#pragma region Renderer {
//
//const int Renderer::kMaxFramesInFlight = 3;
//
//Renderer::Renderer( MTL::Device* pDevice )
//		: _pDevice( pDevice->retain() )
//		, _angle ( 0.f )
//		, _frame( 0 )
//{
//	_pCommandQueue = _pDevice->newCommandQueue();
//	buildShaders();
//	buildBuffers();
//
//	_semaphore = dispatch_semaphore_create( Renderer::kMaxFramesInFlight );
//}
//
//Renderer::~Renderer()
//{
//	_pShaderLibrary->release();
//	_pVertexDataBuffer->release();
//	for ( int i = 0; i < kMaxFramesInFlight; ++i )
//	{
//		_pInstanceDataBuffer[i]->release();
//	}
//	_pIndexBuffer->release();
//	_pPSO->release();
//	_pCommandQueue->release();
//	_pDevice->release();
//}
//
//
//void Renderer::buildShaders()
//{
//	using NS::StringEncoding::UTF8StringEncoding;
//
//	const char* shaderSrc = R"(
//        #include <metal_stdlib>
//        using namespace metal;
//
//        struct v2f
//        {
//            float4 position [[position]];
//            half3 color;
//        };
//
//        struct VertexData
//        {
//            float3 position;
//        };
//
//        struct InstanceData
//        {
//            float4x4 instanceTransform;
//            float4 instanceColor;
//        };
//
//        v2f vertex vertexMain( device const VertexData* vertexData [[buffer(0)]],
//                               device const InstanceData* instanceData [[buffer(1)]],
//                               uint vertexId [[vertex_id]],
//                               uint instanceId [[instance_id]] )
//        {
//            v2f o;
//            float4 pos = float4( vertexData[ vertexId ].position, 1.0 );
//            o.position = instanceData[ instanceId ].instanceTransform * pos;
//            o.color = half3( instanceData[ instanceId ].instanceColor.rgb );
//            return o;
//        }
//
//        half4 fragment fragmentMain( v2f in [[stage_in]] )
//        {
//            return half4( in.color, 1.0 );
//        }
//    )";
//
//	NS::Error* pError = nullptr;
//	MTL::Library* pLibrary = _pDevice->newLibrary( NS::String::string(shaderSrc, UTF8StringEncoding), nullptr, &pError );
//	if ( !pLibrary )
//	{
//		__builtin_printf( "%s", pError->localizedDescription()->utf8String() );
//		assert( false );
//	}
//
//	MTL::Function* pVertexFn = pLibrary->newFunction( NS::String::string("vertexMain", UTF8StringEncoding) );
//	MTL::Function* pFragFn = pLibrary->newFunction( NS::String::string("fragmentMain", UTF8StringEncoding) );
//
//	MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
//	pDesc->setVertexFunction( pVertexFn );
//	pDesc->setFragmentFunction( pFragFn );
//	pDesc->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
//
//	_pPSO = _pDevice->newRenderPipelineState( pDesc, &pError );
//	if ( !_pPSO )
//	{
//		__builtin_printf( "%s", pError->localizedDescription()->utf8String() );
//		assert( false );
//	}
//
//	pVertexFn->release();
//	pFragFn->release();
//	pDesc->release();
//	_pShaderLibrary = pLibrary;
//}
//
//void Renderer::buildBuffers()
//{
//	using simd::float3;
//
//	const float s = 0.5f;
//
//	float3 verts[] = {
//			{ -s, -s, +s },
//			{ +s, -s, +s },
//			{ +s, +s, +s },
//			{ -s, +s, +s }
//	};
//
//	uint16_t indices[] = {
//			0, 1, 2,
//			2, 3, 0,
//	};
//
//	const size_t vertexDataSize = sizeof( verts );
//	const size_t indexDataSize = sizeof( indices );
//
//	printf("%zu!!!!%zu",vertexDataSize, 4*sizeof(float3));
//
//	MTL::Buffer* pVertexBuffer = _pDevice->newBuffer( vertexDataSize, MTL::ResourceStorageModeManaged );
//	MTL::Buffer* pIndexBuffer = _pDevice->newBuffer( indexDataSize, MTL::ResourceStorageModeManaged );
//
//	_pVertexDataBuffer = pVertexBuffer;
//	_pIndexBuffer = pIndexBuffer;
//
//	memcpy( _pVertexDataBuffer->contents(), verts, vertexDataSize );
//	memcpy( _pIndexBuffer->contents(), indices, indexDataSize );
//
//	_pVertexDataBuffer->didModifyRange( NS::Range::Make( 0, _pVertexDataBuffer->length() ) );
//	_pIndexBuffer->didModifyRange( NS::Range::Make( 0, _pIndexBuffer->length() ) );
//
////    size_t instanceDataSize = kNumInstances * sizeof( shader_types::InstanceData )/10;
////	printf("%zu\n",instanceDataSize);
////	instanceDataSize=1;
////
//	for (auto & i : _pInstanceDataBuffer)
//	{
//		i = _pDevice->newBuffer( 1, MTL::ResourceStorageModeManaged );
//	}
//}
//
//void Renderer::draw( MTK::View* pView )
//{
//	using simd::float4;
//	using simd::float4x4;
//
//	NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
//
//	_frame = (_frame + 1) % Renderer::kMaxFramesInFlight;
//	MTL::Buffer* pInstanceDataBuffer = _pInstanceDataBuffer[ _frame ];
//
//	MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
//	dispatch_semaphore_wait( _semaphore, DISPATCH_TIME_FOREVER );
//	Renderer* pRenderer = this;
//	pCmd->addCompletedHandler( ^void( MTL::CommandBuffer* pCmd ){
//		dispatch_semaphore_signal( pRenderer->_semaphore );
//	});
//
//	_angle += 0.01f;
//
//	const float scl = 0.1f;
//	shader_types::InstanceData* pInstanceData = reinterpret_cast< shader_types::InstanceData *>( pInstanceDataBuffer->contents() );
//	for ( size_t i = 0; i < _pNumInstances; ++i )
//	{
//		float iDivNumInstances = i / (float)_pNumInstances;
//		float xoff = (iDivNumInstances * 2.0f - 1.0f) + (1.f/_pNumInstances);
//		float yoff = sin( ( iDivNumInstances + _angle ) * 2.0f * M_PI);
//		pInstanceData[ i ].instanceTransform = (float4x4){ (float4){ scl * sinf(_angle), scl * cosf(_angle), 0.f, 0.f },
//														   (float4){ scl * cosf(_angle), scl * -sinf(_angle), 0.f, 0.f },
//														   (float4){ 0.f, 0.f, scl, 0.f },
//														   (float4){ xoff, yoff, 0.f, 1.f } };
////		pInstanceData[ i ].instanceTransform = (float4x4){ (float4){ 0.1, 0, 0.f, 0.f },
////														   (float4){ 0, 0.1, 0.f, 0.f },
////														   (float4){ 0.f, 0.f, 0, 0.f },
////														   (float4){ xoff, yoff, 0.f, 1.f } };
//
//		float r = iDivNumInstances;
//		float g = 1.0f - r;
//		float b = sinf( M_PI * 2.0f * iDivNumInstances );
//		pInstanceData[ i ].instanceColor = (float4){ r, g, b, 1.0f };
//	}
//	pInstanceDataBuffer->didModifyRange( NS::Range::Make( 0, pInstanceDataBuffer->length() ) );
//
//
//	MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
//	MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
//
//	pEnc->setRenderPipelineState( _pPSO );
//	pEnc->setVertexBuffer( _pVertexDataBuffer, /* offset */ 0, /* index */ 0 );
//	pEnc->setVertexBuffer( pInstanceDataBuffer, /* offset */ 0, /* index */ 1 );
//
//	//
//	// void drawIndexedPrimitives( PrimitiveType primitiveType, NS::UInteger indexCount, IndexType indexType,
//	//                             const class Buffer* pIndexBuffer, NS::UInteger indexBufferOffset, NS::UInteger instanceCount );
//	pEnc->drawIndexedPrimitives( MTL::PrimitiveType::PrimitiveTypeTriangle,
//								 6, MTL::IndexType::IndexTypeUInt16,
//								 _pIndexBuffer,
//								 0,
//								 _pNumInstances );
//
//	pEnc->endEncoding();
//	pCmd->presentDrawable( pView->currentDrawable() );
//	pCmd->commit();
//
//	pPool->release();
//}
//
//#pragma endregion Renderer }