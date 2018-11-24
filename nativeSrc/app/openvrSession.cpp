#include <openvr.h>
#include "j.h"

class OpenVRSession {
public:
	OpenVRSession() {
    }
    bool init(){
        if(!vr::VR_IsHmdPresent()){
            jlog("VR device not found!");
            return false;
        }

        jlog("VR device found");
        vr::EVRInitError eError = vr::VRInitError_None;
        this->m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

        if ( eError != vr::VRInitError_None )
        {
            jlog("Unable to init device");
            return false;
        }

        m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &eError );
        if( !m_pRenderModels )
        {
            m_pHMD = NULL;
            jlog("Unable to get render model interface");
            //vr::VR_Shutdown();
            return false;
        }
        
        jlog("vr Ready");
        return true;
    }
private:
    vr::IVRSystem *m_pHMD;
    vr::IVRRenderModels *m_pRenderModels;
    vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
};