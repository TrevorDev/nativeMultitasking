#pragma once

#include "j.h"
#include "vulkanInc.h"
#include "device.cpp"
#include "renderPass.cpp"
class Image {
    public:
    vk::Image _image;
    vk::DeviceMemory _imageMemory;
    vk::ImageView _imageView;
    vk::Framebuffer _framebuffer;
    Device _device;
    Image(){
        
    }

    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags) {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        _imageView = _device._device.createImageView(viewInfo);
    }

    void createFrameBuffer(Image depthImage, RenderPass _renderPass, int width, int height){
        // create framebuffer
        std::array<VkImageView, 2> attachments = {
            _imageView,
            depthImage._imageView
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass._renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        _framebuffer = _device._device.createFramebuffer(framebufferInfo);
    }

    HANDLE createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
        // VkExternalMemoryImageCreateInfo extMemor = {};
        // extMemor.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
        // extMemor.handleTypes = VkExternalMemoryHandleTypeFlagBits::VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
        
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //imageInfo.pNext = &extMemor;

        _image = _device._device.createImage(imageInfo);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(_device._device, _image, &memRequirements);
        
        // https://docs.microsoft.com/en-us/windows/desktop/SecAuthZ/sid-strings
        // https://docs.microsoft.com/en-us/windows/desktop/SecBP/creating-a-dacl
        // TODO to allow external processes https://stackoverflow.com/questions/46348163/how-to-transfer-the-duplicated-handle-to-the-child-process
        // For now setting bInheritHandle auto allows child processes to work
        // SECURITY_ATTRIBUTES  sa;
        // sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        // sa.bInheritHandle = TRUE;
        
        // TCHAR * szSD = TEXT("D:")       // Discretionary ACL

        // TEXT("(A;OICI;GRGWGX;;;WD)") // Allow 
        //                              // read/write/execute 
        //                              // to all users
        // //TEXT("(A;OICI;GA;;;LA)")
        // TEXT("(A;OICI;GA;;;WD)");    // Allow full control 
        //                              // to administrators  

        // std::cout << szSD << std::endl;

        // std::cout << ConvertStringSecurityDescriptorToSecurityDescriptor(
        //         szSD,
        //         SDDL_REVISION_1,
        //         &((&sa)->lpSecurityDescriptor),
        //         NULL) << std::endl;
        // // Allocate


        
        // // TODO secAttr needs to support external app instead of child process
        // VkExportMemoryWin32HandleInfoKHR winExpInfo = {};
        // winExpInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
        // winExpInfo.dwAccess = GENERIC_ALL;
        // winExpInfo.pAttributes = &sa;
        //winExpInfo.name = L"testExp";
        
        // VkExportMemoryAllocateInfo expAllocInfo = {};
        // expAllocInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
        //expAllocInfo.handleTypes = VkExternalMemoryHandleTypeFlagBits::VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
        //expAllocInfo.pNext = &winExpInfo;

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = _device.findMemoryType(memRequirements.memoryTypeBits, properties);
        //allocInfo.pNext = &expAllocInfo;
        
        _imageMemory = _device._device.allocateMemory(allocInfo);

        vkBindImageMemory(_device._device, _image, _imageMemory, 0);


        // GET HANDLE
        // auto vkGetMemoryWin32HandleKHR = PFN_vkGetMemoryWin32HandleKHR(vkGetDeviceProcAddr(_device, "vkGetMemoryWin32HandleKHR"));
        // VkMemoryGetWin32HandleInfoKHR info = {};
        // info.sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR;
        // info.handleType = VkExternalMemoryHandleTypeFlagBits::VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
        // info.memory = imageMemory;
        HANDLE fd = nullptr;
        
        //VkResult r = vkGetMemoryWin32HandleKHR(_device, &info, &fd);
        //std::cout << fd << std::endl;
        
        return fd;
    }
    
    private:
    
};