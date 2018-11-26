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

        if(!vr::VRCompositor()){
            jlog("compositor failed to load");
            return false;
        }

        jlog("vr Ready");
        return true;
    }

    VkPhysicalDevice getDesiredVulkanDevice(VkInstance_T * inst){
        uint64_t pHMDPhysicalDevice = 0;
	    m_pHMD->GetOutputDevice( &pHMDPhysicalDevice, vr::TextureType_Vulkan, ( VkInstance_T * ) inst );
        return (VkPhysicalDevice)pHMDPhysicalDevice;
    }

    bool getVulkanInstanceExtensionsRequired(std::vector<std::string>& vec){
        if ( !vr::VRCompositor() )
        {
            return false;
        }

        uint32_t nBufferSize = vr::VRCompositor()->GetVulkanInstanceExtensionsRequired( nullptr, 0 );
        if ( nBufferSize > 0 )
        {
            // Allocate memory for the space separated list and query for it
            char *pExtensionStr = new char[ nBufferSize ];
            pExtensionStr[0] = 0;
            vr::VRCompositor()->GetVulkanInstanceExtensionsRequired( pExtensionStr, nBufferSize );

            // Break up the space separated list into entries on the CUtlStringList
            std::string curExtStr;
            uint32_t nIndex = 0;
            while ( pExtensionStr[ nIndex ] != 0 && ( nIndex < nBufferSize ) )
            {
                if ( pExtensionStr[ nIndex ] == ' ' )
                {
                    vec.push_back( curExtStr );
                    curExtStr.clear();
                }
                else
                {
                    curExtStr += pExtensionStr[ nIndex ];
                }
                nIndex++;
            }
            if ( curExtStr.size() > 0 )
            {
                vec.push_back( curExtStr );
            }

            delete [] pExtensionStr;
        }

        return true;
    }


private:
    vr::IVRSystem *m_pHMD;
    vr::IVRRenderModels *m_pRenderModels;
    vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
};