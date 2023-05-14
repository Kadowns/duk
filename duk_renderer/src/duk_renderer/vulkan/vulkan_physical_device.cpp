//
// Created by Ricardo on 13/04/2023.
//

#include <duk_renderer/vulkan/vulkan_physical_device.h>

#include <stdexcept>

namespace duk::renderer {


VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanPhysicalDeviceCreateInfo& physicalDeviceCreateInfo) :
    m_physicalDevice(VK_NULL_HANDLE){
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(physicalDeviceCreateInfo.instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find a physical device on VkInstance");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(physicalDeviceCreateInfo.instance, &deviceCount, devices.data());

    auto desiredGPUIndex = (size_t)physicalDeviceCreateInfo.deviceIndex;
    if (desiredGPUIndex >= devices.size()){
        throw std::runtime_error("physical device index out of bounds");
    }

    m_physicalDevice = devices[desiredGPUIndex];

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

    m_queueFamilyProperties.resize(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_queueFamilyProperties.data());

}

VulkanPhysicalDevice::~VulkanPhysicalDevice() = default;

ExpectedVulkanQueueFamilyProperties VulkanPhysicalDevice::find_queue_family(VkSurfaceKHR surface, VkQueueFlags requiredQueueFlags, VkQueueFlags prohibitedQueueFlags) const {
    for (uint32_t candidateQueueIndex = 0; candidateQueueIndex < m_queueFamilyProperties.size(); candidateQueueIndex++) {

        const auto& candidateQueueFamily = m_queueFamilyProperties[candidateQueueIndex];

        if (surface) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, candidateQueueIndex, surface, &presentSupport);

            if (!presentSupport) {
                continue;
            }
        }

        auto candidateFlags = candidateQueueFamily.queueFlags;

        if (candidateQueueFamily.queueCount > 0 && candidateFlags & requiredQueueFlags && (candidateFlags & prohibitedQueueFlags) == 0) {
            return VulkanQueueFamilyProperties{candidateQueueFamily, candidateQueueIndex};
        }
    }

    return tl::unexpected<VulkanQueryError>(VulkanQueryError::NOT_FOUND);
}

ExpectedVulkanSurfaceDetails VulkanPhysicalDevice::query_surface_details(VkSurfaceKHR surface) const {
    if (!surface) {
        return tl::unexpected<VulkanQueryError>(VulkanQueryError::INVALID_ARGUMENT);
    }

    VulkanSurfaceDetails surfaceDetails = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, surface, &surfaceDetails.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        surfaceDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &formatCount, surfaceDetails.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        surfaceDetails.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &presentModeCount, surfaceDetails.presentModes.data());
    }

    return surfaceDetails;
}

VkPhysicalDevice VulkanPhysicalDevice::handle() {
    return m_physicalDevice;
}

const std::vector<VkQueueFamilyProperties>& VulkanPhysicalDevice::queue_family_properties() const {
    return m_queueFamilyProperties;
}

}