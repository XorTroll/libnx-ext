
#pragma once
#include "cruiser/cruiser-cpp/cruiser.hpp"
extern "C" {
    #include "cruiser/cruiser/wkc-types.h"
}

namespace WKC {

    // https://github.com/syvb/switch-oss/blob/master/WKC_2.26.0/WebKit/WKC/webkit/WKCMemoryEventHandler.h

    /**
    @brief Class that notifies of memory-related events
    */
    class WKCMemoryEventHandler
    {
    public:
        // checks whether requested size of memory is available.
        /**
         @brief Checks whether memory allocation is allowed
        @param request_size Requested size of memory to allocate
        @param in_forimage Requested type @n
        - != false Image memory allocation
        - == false Other than memory allocation
        @retval "!= false" Allocation allowed
        @retval "== false" Allocation not allowed
        */
        virtual bool checkMemoryAvailability(size_t request_size, bool in_forimage) = 0;

        // check whether requested size of something should be allocatable.
        /** @brief Reason for memory allocation */
        enum AllocationReason {
            /** @brief Image memory allocation */
            Image,
            /** @brief Allocate for decoding of JavaScript text */
            JavaScript,
            /** @brief Allocate from Cairo and Pixman. */
            Pixman,
        };
        /**
         @brief Checks whether memory allocation is allowed
        @param request_size Requested size of memory to allocate
        @param reason Reason for memory allocation
        @retval "!= false" Allocation allowed
        @retval "== false" Allocation not allowed
        @details
        If false is specified for the return value, processing will continue without memory allocation.
        */
        virtual bool checkMemoryAllocatable(size_t request_size, AllocationReason reason) = 0;

        // notifies memory exhaust event.
        // arguments and return value are for future implementations.
        // at this point please do exit / finalize application.
        /**
         @brief Notifies of memory allocation failure
        @param request_size Requested memory size
        @param out_allocated_size Reference to allocated memory size
        @retval void* Pointer to allocated memory
        */
        virtual void* notifyMemoryExhaust(size_t request_size, size_t& out_allocated_size) = 0;

        // notifies memory exhaust event at allocation.
        /**
         @brief Notifies of memory allocation failure
        @param request_size Requested memory size
        @param reason Reason for memory allocation
        @return None
        @details
        Notification is given when memory allocation fails.
        */
        virtual void notifyMemoryAllocationError(size_t request_size, AllocationReason reason) = 0;
        
        // notifies some CRASH() event
        // applications should do exit at this calling.
        /**
         @brief Notifies of forced termination
        @param file (When assert fails) path (C string) to file where assertion failed
        @param line (When assert fails) line number where assertion failed
        @param function (When assert fails) function (ASCII-only C string) where assertion failed
        @param assertion (When assert fails) assertion (ASCII-only C string) that failed
        @return None
        @details
        Notification is given when the browser engine is forcibly terminated.@n
        In the case of forced termination for reasons other than assertion failure, file, line, function, and assertion must be set to 0.
        @attention
        The file path to the build environment file path is specified for file when there is an assertion failure.@n
        If the file path contains Japanese, etc., it will be in the platform-dependent character encoding, but if there is only ASCII in the file path, then file will be an ASCII-only string.
        */
        virtual void notifyCrash(const char* file, int line, const char* function, const char* assertion) = 0;

        // notifies stack overflow.
        // applications should do exit at this calling if need_restart is true.
        /**
        @brief Notifies of stack overflow
        @param need_restart Notification whether restart is required
        - true  Restart is required.
        - false Restart is not required.
        @param stack_size Stack size
        @param consumption Stack consumption
        @param margin Margin
        @param stack_top Top address of stack area
        @param stack_base Bottom address of stack area
        @param current_stack_top Current top stack address
        @param file Path (C string) of file with process where stack overflow occurred
        @param line Line number in process where stack overflow occurred
        @param function Function (ASCII-only or C string) with process where stack overflow occurred
        @return None
        @details
        Notification is given when stack overflow appears about to occur in the browser engine.@n
        The stack size set by WKC::WKCWebKitSetStackSize() is used as the standard.@n
        It is called when the value after subtracting stack consumption from stack size becomes smaller than the margin. The appropriate value for each process inside the browser engine is applied for the margin.
        @attention
        Cases when need_restart is notified as true are when the HTML content is extremely complicated (there is an extreme number of nested elements, etc.).
        In these cases, basically it is not possible to continue processing in the browser engine, similar to when there is insufficient memory.@n
        Cases when need_restart is notified as false are when the stack becomes insufficient while running JavaScript.
        In these cases, since the behavior itself can continue if you stop JavaScript, the engine will continue to operate as long as the restart process is not performed on the application side, however it is okay to restart if the application side decides to.@n
        The build environment file path is specified for file.@n
        If the file path contains Japanese, etc., it will be in the platform-dependent character encoding, but if there is only ASCII in the file path, then file will be an ASCII-only string.
        */
        virtual void notifyStackOverflow(bool need_restart, size_t stack_size, size_t consumption, size_t margin, void* stack_top, void* stack_base, void* current_stack_top, const char* file, int line, const char* function) = 0;

        /**
         @brief Allocates physical pages for the specified virtual adress region
        @param ptr Pointer to the virtual address region
        @param size Size of the virtual address region
        @retval "!= false" Succeeded in allocating physical pages
        @retval "== false" Failed to allocating physical pages
        */
        virtual bool allocateMemoryPages(void* ptr, size_t size) = 0;

        /**
         @brief Frees the physical pages for the specified virtual adress region
        @param ptr Pointer to the virtual address region
        @param size Size of the virtual address region
        */
        virtual void freeMemoryPages(void* ptr, size_t size) = 0;
    };

    // https://github.com/syvb/switch-oss/blob/master/WKC_2.26.0/WebKit/WKC/webkit/WKCTimerEventHandler.h

    /** @brief Class that notifies of timer-related events */
    class WKCTimerEventHandler
    {
    public:
        // requests to call WKCWebKitWakeUp from browser thread.
        /**
         @brief Requests timer firing
        @param in_timer Peer Timer instance
        @param in_proc callback function
        @param in_data Data to be used in in_proc argument
        @retval "!= false" Succeeded in setting timer
        @retval "== false" Failed to set timer
        @details Notifies of timer firing requests in order to call in_proc.
        */
        virtual bool requestWakeUp(void* in_timer, bool(*in_proc)(void*), void* in_data) = 0;

        // cancel to call WKCWebKitWakeUp from browser thread.
        /**
         @brief cancel timer
        @param in_timer Peer Timer instance
        @details Notifies of timer canceling requests in order to call in_proc.
        */
        virtual void cancelWakeUp(void* in_timer) = 0;
    };

    /**
    @brief Initializes browser engine
    @param memory Pointer to heap memory for browser engine
    @param physical_memory_size Size of physical memory for browser engine (bytes)
    @param virtual_memory_size Size of virtual memory for browser engine (bytes)
    @param font_memory Pointer to heap memory for font engine 
    @param font_memory_size Size of heap memory for font engine (bytes)
    @param memory_event_handler Reference to WKC::WKCMemoryEventHandler
    @param timer_event_handler Reference to WKC::WKCTimerEventHandler
    @retval "!= false" Succeeded in initializing
    @retval "== false" Failed to initialize
    @details
    The heap memory for the browser engine must be set to a value of 10MB or higher.@n
    If virtual_memory_size is not zero, the browser engine runs on the virtual memory system.
    In that case, memory, physical_memory_size and virtual_memory_size must be aligned on your system page size.
    The heap memory for the font engine must be set to an appropriate value of 256KB or higher, or 0(in case disable font engine).
    Considering trade-offs with performance, 512KB is the recommended value for the size.
    If font_memory and its size is 0, the font engine become disabled and no text drawn.
    @attention
    The method of setting heap memory for the font engine may change in the future.
    */
    CRUISER_API(bool (*WKCWebKitInitialize)(void* memory, size_t physical_memory_size, size_t virtual_memory_size, void* font_memory, size_t font_memory_size, WKCMemoryEventHandler& memory_event_handler, WKCTimerEventHandler& timer_event_handler));

    /**
    @brief Terminates browser engine
    @return None
    */
    CRUISER_API(void (*WKCWebKitFinalize)());

    /** @brief Structure for storing system string data */
    struct SystemStrings {
        /** @brief platform property of JavaScript Navigator object */
        const unsigned short* fNavigatorPlatform;
        /** @brief product property of JavaScript Navigator object */
        const unsigned short* fNavigatorProduct;
        /** @brief productSub property of JavaScript Navigator object */
        const unsigned short* fNavigatorProductSub;
        /** @brief vendor property of JavaScript Navigator object */
        const unsigned short* fNavigatorVendor;
        /** @brief vendorSub property of JavaScript Navigator object */
        const unsigned short* fNavigatorVendorSub;
        /** @brief System language */
        const unsigned short* fLanguage;
        /** @brief Button text of input element for which type="submit" is specified */
        const unsigned short* fButtonLabelSubmit;
        /** @brief Button text of input element for which type="reset" is specified */
        const unsigned short* fButtonLabelReset;
        /** @brief Button text of input element for which type="file" is specified */
        const unsigned short* fButtonLabelFile;
    };

    namespace WKCPrefs {

        // https://github.com/syvb/switch-oss/blob/master/WKC_2.26.0/WebKit/WKC/webkit/WKCPrefs.h

        /**
        @brief Sets engine thread information
        @param thread_id Thread ID
        @param stack_base Stack base address
        @retval None
        @details
        Sets ID and stack back address of thread that calls a browser engine API.@n
        This API does not have to be called on platforms where the thread and stack base address can be obtained by the porting layer alone.@n
        @attention
        On platforms other than those where the thread and stack base address can be obtained by the porting layer alone, this must always be called and set when creating the browser instance.
        */
        CRUISER_API(void (*setThreadInfo)(void* threadID, void* stackBase));

        /**
        @brief Sets stack size
        @param stack_size Stack size
        @retval None
        @details
        The stack size set by this function is used as the stack size for the stack overflow detection process inside the browser engine and that notified by WKC::WKCMemoryEventHandler::notifyStackOverflow().
        @attention
        Always call and set this when creating the browser instance.
        */
        CRUISER_API(void (*setStackSize)(unsigned int in_stack_size));

        /**
        @brief Sets system strings
        @param strings System strings
        @retval None
        @details
        The strings set by this function are copied internally.
        @attention
        Always call and set this when creating the browser instance. @n
        If this function is not called, or if NULL is set for a member of WKC::SystemStrings and called, then the following values will be set as default values for system strings. @n
        - platform property of JavaScript Navigator object (WKC::SystemStrings::fNavigatorPlatform) … "" (empty string)
        - product property of JavaScript Navigator object (WKC::SystemStrings::fNavigatorProduct) … "" (empty string)
        - productSub property of JavaScript Navigator object (WKC::SystemStrings::fNavigatorProductSub) … "" (empty string)
        - vendor property of JavaScript Navigator object (WKC::SystemStrings::fNavigatorVendor) … "" (empty string)
        - vendorSub property of JavaScript Navigator object (WKC::SystemStrings::fNavigatorVendorSub) … "" (empty string)
        - System language (WKC::SystemStrings::fLanguage) … " en "
        - Button text of input element for which type="submit" is specified (WKC::SystemStrings::fButtonLabelSubmit) … "Submit"
        - Button text of input element for which type="reset" is specified (WKC::SystemStrings::fButtonLabelReset) … "Reset"
        - Button text of input element for which type="file" is specified (WKC::SystemStrings::fButtonLabelFile) … "Choose File"
        */
        CRUISER_API(void (*setSystemStrings)(const SystemStrings *strings));

    }

    // https://github.com/syvb/switch-oss/blob/master/WKC_2.26.0/WebKit/WKC/webkit/WKCWebView.h

    CRUISER_API(void (*WKCWebKitSetMediaPlayerProcs)(const WKCMediaPlayerProcs* procs));
    CRUISER_API(void (*WKCWebKitSetThreadProcs)(const WKCThreadProcs* procs));

    namespace Heap {

        // https://github.com/syvb/switch-oss/tree/b7f486be1210b9f6d216d334e472b56bee818923/WKC_2.26.0/WebKit/WKC/webkit/WKCMemoryInfo.h

        /**
        @brief Gets amount of browser engine heap available
        @retval size_t Size of total free memory (bytes)
        */
        CRUISER_API(size_t (*GetAvailableSize)());

        /**
        @brief Gets maximum size of browser engine heap that can be allocated
        @retval size_t Maximum size that can be allocated (bytes)
        */
        CRUISER_API(size_t (*GetMaxAvailableBlockSize)());

        /**
        @brief Sets default capacity of JavaScript register file
        @param size size of default capacity
        @details
        Sets default capacity of JavaScript register file
        */
        CRUISER_API(void (*SetJSRegisterFileDefaultCapacity)(unsigned int size));

        /**
        @brief Gets size of memory allocatad for JavaScript heap from engine heap
        @return size_t Size of memory allocatad for JavaScript heap (bytes)
        */
        CRUISER_API(size_t (*GetJSHeapAllocatedBlockBytes)());

        CRUISER_API(size_t (*GetStatisticsFreeSizeInHeap)());

        /** @brief Structure for storing memory usage statistics of engine heap (TCMalloc) */
        struct Statistics {
            /** @brief Consuming size */
            size_t heapSize;
            /** @brief Total size of free memory in CentralCache and ThreadCache */
            size_t freeSizeInHeap;
            /** @brief Total size of free memory in TCMalloc_PageHeap */
            size_t freeSizeInCaches;
            /** @brief Size of memory freed by TCMalloc_SystemRelease() */
            size_t returnedSize;
            /** @brief Page size set in TCMalloc */
            size_t pageSize;
            /** @brief Size of max free block in TCMalloc_PageHeap */
            size_t maxFreeBlockSizeInHeap;
            /** @brief Total size of free memory in CentralCache and ThreadCache of the class ID specified by classInCaches */
            size_t classFreeSizeInCaches;
            /** @brief Size of memory block of the class ID specified by classInCaches */
            size_t classBlockSizeInCaches;
            /** @brief Amount of pages required to allocate requested size of memory (see details of GetStatistics()) */
            unsigned int pageLength;
            /** @brief Class ID corresponding to the requested size (see details of GetStatistics()) */
            unsigned int classInCaches;
            /** @brief Amount of classes */
            size_t numClasses;
            /** @brief Amount of pages assigned to each class ID */
            size_t* eachClassAssignedPagesInCaches;
            /** @brief Size of free memory in ThreadCache of each class ID */
            size_t* eachClassThreadFreeSizeInCaches;
            /** @brief Size of free memory in CentralCache of each class ID */
            size_t* eachClassCentralFreeSizeInCaches;
            /** @brief Size of memory block of each class ID */
            size_t* eachClassBlockSizeInCaches;
            /** @brief  */
            size_t maxPages;
            /** @brief Size of free memory in each page */
            size_t* eachPageFreeSizeInHeap;
            /** @brief Size of array of largeFreeSizeInHeap */
            unsigned int numLargeFreeSize;
            /** @brief Max size of heap */
            size_t maxHeapSize;
            /** @brief Size of free memory in combined blocks of pages over pageSize */
            size_t* largeFreeSizeInHeap;
            /** @brief Current heap usage (See note) */
            size_t currentHeapUsage;
            /** @brief Current physical memory usage */
            size_t currentPhysicalMemoryUsage;
            /** @brief Maximum heap usage (See note) */
            size_t maxHeapUsage;
            size_t allocFailureCount;
            size_t allocFailureMinSize;
            size_t allocFailureTotalSize;
            /* - note -
            * It does not include the amount of heap allocated to the cache in this calculation.
            * The reason is as follows:
            * - Cache size is small.
            * - Calculation process is heavy.
            */
        };

        CRUISER_API(void (*GetStatistics)(Statistics* stat, size_t requestSize));

    }

}
