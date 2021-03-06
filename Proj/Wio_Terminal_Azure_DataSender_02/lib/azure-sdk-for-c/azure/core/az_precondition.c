// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <azure/core/internal/az_precondition_internal.h>
#include <stdint.h>

#include <azure/core/_az_cfg.h>

static void az_precondition_failed_default()
{
  /* By default, when a precondition fails the calling thread spins forever */
  volatile int ctr = 0;
  while (1)
  {
    //RoSchmi
    ctr++;
  }
}

az_precondition_failed_fn _az_precondition_failed_callback = az_precondition_failed_default;

void az_precondition_failed_set_callback(az_precondition_failed_fn az_precondition_failed_callback)
{
  _az_precondition_failed_callback = az_precondition_failed_callback;
}

az_precondition_failed_fn az_precondition_failed_get_callback()
{
  return _az_precondition_failed_callback;
}
