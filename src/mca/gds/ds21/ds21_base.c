/*
 * Copyright (c) 2015-2018 Intel, Inc. All rights reserved.
 * Copyright (c) 2016      IBM Corporation.  All rights reserved.
 * Copyright (c) 2016-2018 Mellanox Technologies, Inc.
 *                         All rights reserved.
 * Copyright (c) 2018      Research Organization for Information Science
 *                         and Technology (RIST).  All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/pmix_config.h>
#include <pmix_common.h>
#include "src/include/pmix_globals.h"
#include "src/util/error.h"
#include "src/mca/gds/base/base.h"

#include "src/mca/common/dstore/dstore_common.h"
#include "ds21_base.h"
#include "ds21_lock.h"
#include "src/mca/common/dstore/dstore_base.h"

static pmix_common_dstore_ctx_t *ds21_ctx;

static pmix_status_t ds21_init(pmix_info_t info[], size_t ninfo)
{
    pmix_status_t rc = PMIX_SUCCESS;

    ds21_ctx = pmix_common_dstor_init(&pmix_ds21_lock_module, "ds21", info, ninfo);
    if (NULL == ds21_ctx) {
        rc = PMIX_ERR_INIT;
    }

    return rc;
}

static void ds21_finalize(void)
{
    pmix_common_dstor_finalize(ds21_ctx);
}

static pmix_status_t ds21_assign_module(pmix_info_t *info, size_t ninfo,
                                        int *priority)
{
    return pmix_common_dstor_assign_module(ds21_ctx, info, ninfo, priority);
}

static pmix_status_t ds21_cache_job_info(struct pmix_nspace_t *ns,
                                pmix_info_t info[], size_t ninfo)
{
    return PMIX_SUCCESS;
}

static pmix_status_t ds21_register_job_info(struct pmix_peer_t *pr,
                                            pmix_buffer_t *reply)
{
    return pmix_common_dstor_register_job_info(ds21_ctx, pr, reply);
}

static pmix_status_t ds21_store_job_info(const char *nspace,  pmix_buffer_t *buf)
{
    return pmix_common_dstor_store_job_info(ds21_ctx, nspace, buf);
}

static pmix_status_t ds21_store(const pmix_proc_t *proc,
                                    pmix_scope_t scope,
                                    pmix_kval_t *kv)
{
    return pmix_common_dstor_store(ds21_ctx, proc, scope, kv);
}

/* this function is only called by the PMIx server when its
 * host has received data from some other peer. It therefore
 * always contains data solely from remote procs, and we
 * shall store it accordingly */
static pmix_status_t ds21_store_modex(struct pmix_nspace_t *nspace,
                                      pmix_list_t *cbs,
                                      pmix_byte_object_t *bo)
{
    return pmix_common_dstor_store_modex(ds21_ctx, nspace, cbs, bo);
}

static pmix_status_t ds21_fetch(const pmix_proc_t *proc,
                                    pmix_scope_t scope, bool copy,
                                    const char *key,
                                    pmix_info_t info[], size_t ninfo,
                                    pmix_list_t *kvs)
{
    return pmix_common_dstor_fetch(ds21_ctx, proc, scope, copy, key, info, ninfo, kvs);
}

static pmix_status_t ds21_setup_fork(const pmix_proc_t *peer, char ***env)
{
    pmix_status_t rc;
    char *env_name = NULL;
    int ds_ver = 0;

    sscanf(ds21_ctx->ds_name, "ds%d", &ds_ver);
    if (0 == ds_ver) {
        rc = PMIX_ERR_INIT;
        PMIX_ERROR_LOG(rc);
        return rc;
    }
    if (0 > asprintf(&env_name, PMIX_DSTORE_VER_BASE_PATH_FMT, ds_ver)) {
         rc = PMIX_ERR_NOMEM;
         PMIX_ERROR_LOG(rc);
         return rc;
    }
    rc = pmix_common_dstor_setup_fork(ds21_ctx, env_name, peer, env);
    free(env_name);

    return rc;
}

static pmix_status_t ds21_add_nspace(const char *nspace,
                                pmix_info_t info[],
                                size_t ninfo)
{
    return pmix_common_dstor_add_nspace(ds21_ctx, nspace, info, ninfo);
}

static pmix_status_t ds21_del_nspace(const char* nspace)
{
    return pmix_common_dstor_del_nspace(ds21_ctx, nspace);
}

pmix_gds_base_module_t pmix_ds21_module = {
    .name = "ds21",
    .init = ds21_init,
    .finalize = ds21_finalize,
    .assign_module = ds21_assign_module,
    .cache_job_info = ds21_cache_job_info,
    .register_job_info = ds21_register_job_info,
    .store_job_info = ds21_store_job_info,
    .store = ds21_store,
    .store_modex = ds21_store_modex,
    .fetch = ds21_fetch,
    .setup_fork = ds21_setup_fork,
    .add_nspace = ds21_add_nspace,
    .del_nspace = ds21_del_nspace,
};

