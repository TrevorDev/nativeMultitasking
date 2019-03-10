#pragma once
#include "vulkanInc.hpp"
#include "device.cpp"
#include "renderPassAttachment.cpp"
class RenderPass {
    public:
    vk::RenderPass _renderPass;
    RenderPass(){
    }
    void init(Device device, VkFormat color,VkFormat depth) {
        // color
        RenderPassAttachment colorAttach = RenderPassAttachment(color);

        // depth
        RenderPassAttachment depthAttach = RenderPassAttachment(depth);
        depthAttach.attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttach.attachmentRef.attachment = 1;
        depthAttach.attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // subpass and dependencys
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttach.attachmentRef;
        subpass.pDepthStencilAttachment = &depthAttach.attachmentRef;
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Create renderpass with color+depth
        std::array<VkAttachmentDescription, 2> attachments = {colorAttach.attachment, depthAttach.attachment};
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        _renderPass = device._device.createRenderPass(renderPassInfo);
        jlog("render pass created!");
    }
    
    private:
    
};