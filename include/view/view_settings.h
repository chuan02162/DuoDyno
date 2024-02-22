//
// Created by 韩昱川 on 2/19/24.
//

#ifndef DUODYNO_VIEW_SETTINGS_H
#define DUODYNO_VIEW_SETTINGS_H

#endif //DUODYNO_VIEW_SETTINGS_H

#include<simd/simd.h>

// Max frames waiting to render
namespace view_settings{
	static const size_t kMaxFramesInFlight = 3;
	static const size_t kMaxInstances = 1e6;
}