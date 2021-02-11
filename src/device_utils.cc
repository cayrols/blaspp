// Copyright (c) 2017-2020, University of Tennessee. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// This program is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#include "blas/device.hh"

#include "device_internal.hh"

namespace blas {

// -----------------------------------------------------------------------------
// set device
void set_device(blas::Device device)
{
    #ifdef BLAS_HAVE_CUBLAS
        blas_dev_call(
            cudaSetDevice((device_blas_int)device) );

    #elif defined(BLAS_HAVE_ROCBLAS)
        blas_dev_call(
            hipSetDevice((device_blas_int)device) );

    #elif defined(BLAS_HAVE_ONEMKL)
        throw blas::Error( "unsupported function for sycl backend", __func__ );

    #else
        throw blas::Error( "device BLAS not available", __func__ );
    #endif
}

// -----------------------------------------------------------------------------
// get current device
void get_device(blas::Device *device)
{
    device_blas_int dev = -1;
    #ifdef BLAS_HAVE_CUBLAS
        blas_dev_call(
            cudaGetDevice(&dev) );

    #elif defined(BLAS_HAVE_ROCBLAS)
        blas_dev_call(
            hipGetDevice(&dev) );

    #elif defined(BLAS_HAVE_ONEMKL)
        throw blas::Error( "unsupported function for sycl backend", __func__ );
        blas_unused( dev );

    #else
        throw blas::Error( "device BLAS not available", __func__ );
        blas_unused( dev );
    #endif
    (*device) = (blas::Device)dev;
}

// -----------------------------------------------------------------------------
// @return number of GPU devices
device_blas_int get_device_count()
{
    device_blas_int dev_count = 0;

    #ifdef BLAS_HAVE_CUBLAS
        auto err = cudaGetDeviceCount(&dev_count);
        if (err != cudaSuccess && err != cudaErrorNoDevice)
            blas_dev_call( err );

    #elif defined(BLAS_HAVE_ROCBLAS)
        auto err = hipGetDeviceCount(&dev_count);
        if (err != hipSuccess && err != hipErrorNoDevice)
            blas_dev_call( err );

    #elif defined(BLAS_HAVE_ONEMKL)

    auto platforms = platform::get_platforms();
    for (auto &platform : platforms) {
        auto devices = platform.get_devices();
        for (auto &device : devices ) {
            dev_count += device.is_gpu();
        }
    }

    #else
        // return dev_count = 0
    #endif

    return dev_count;
}

// -----------------------------------------------------------------------------
/// free a device pointer
void device_free(void* ptr)
{
    #ifdef BLAS_HAVE_CUBLAS
        blas_dev_call(
            cudaFree( ptr ) );

    #elif defined(BLAS_HAVE_ROCBLAS)
        blas_dev_call(
            hipFree( ptr ) );

    #elif defined(BLAS_HAVE_ONEMKL)
        /*
         * SYCL requires a device/queue to free
         * disable for now
        */
        throw blas::Error( "unsupported function for sycl backend", __func__ );

    #else
        throw blas::Error( "device BLAS not available", __func__ );
    #endif
}

// -----------------------------------------------------------------------------
/// free a device pointer for a given device
void device_free(blas::Device device, void* ptr)
{
    #ifdef BLAS_HAVE_CUBLAS
        blas::set_device( device );
        blas_dev_call(
            cudaFree( ptr ) );

    #elif defined(BLAS_HAVE_ROCBLAS)
        blas::set_device( device );
        blas_dev_call(
            hipFree( ptr ) );

    #elif defined(BLAS_HAVE_ONEMKL)
       cl::sycl::queue tmp_queue( device );
       blas_dev_call(
           sycl::free(ptr, tmp_queue) );
    #endif
}

// -----------------------------------------------------------------------------
/// free a pinned memory space
void device_free_pinned(void* ptr)
{
    #ifdef BLAS_HAVE_CUBLAS
        blas_dev_call(
            cudaFreeHost( ptr ) );

    #elif defined(BLAS_HAVE_ROCBLAS)
        blas_dev_call(
            hipHostFree( ptr ) );

    #elif defined(BLAS_HAVE_ONEMKL)
        throw blas::Error( "unsupported function for sycl backend", __func__ );

    #else
        throw blas::Error( "device BLAS not available", __func__ );
    #endif
}

}  // namespace blas
