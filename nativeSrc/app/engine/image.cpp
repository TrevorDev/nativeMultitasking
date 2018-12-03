#pragma once

#include "j.h"
#include "vulkanInc.h"
#include "device.cpp"
class Image {
    public:
    vk::Image _image;
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
    
    private:
    
};