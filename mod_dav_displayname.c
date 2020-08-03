/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/*
 * The Apache mod_dav_displayname module autopopulates the DAV:displayname
 * property with the name of the resource or collection.
 *
 *  Author: Graham Leggett
 *
 * Basic configuration:
 *
 * <Location />
 *   Dav on
 *   DavDisplayname on
 * </Location>
 *
 */

#include <apr_escape.h>
#include <apr_strings.h>

#include "httpd.h"
#include "http_config.h"

#include "mod_dav.h"

module AP_MODULE_DECLARE_DATA dav_displayname_module;

typedef struct
{
    int dav_displayname_set :1;
    int dav_displayname;
//    ap_expr_info_t *displayname;
} dav_displayname_config_rec;


/* forward-declare the hook structures */
static const dav_hooks_liveprop dav_hooks_liveprop_displayname;

#define DAV_XML_NAMESPACE "DAV:"

/*
** The namespace URIs that we use. This list and the enumeration must
** stay in sync.
*/
static const char * const dav_displayname_namespace_uris[] =
{
    DAV_XML_NAMESPACE,

    NULL        /* sentinel */
};
enum {
    DAV_URI_DAV            /* the DAV: namespace URI */
};

static const dav_liveprop_spec dav_displayname_props[] =
{
    /* standard calendar properties */
    {
        DAV_URI_DAV,
        "displayname",
        DAV_PROPID_displayname,
        0
    },

    { 0 }        /* sentinel */
};

static const dav_liveprop_group dav_displayname_liveprop_group =
{
    dav_displayname_props,
    dav_displayname_namespace_uris,
    &dav_hooks_liveprop_displayname
};

static const char *dav_displayname_render(const dav_resource *resource)
{
    char *uri, *name;
    char *end, *begin;

    apr_pool_t *p = resource->pool;
    uri = apr_pstrdup(p, resource->uri);

    end = strrchr(uri, '/');

    /* no ending slash? name is the URL */
    if (!end) {
        name = uri;
    }

    /* trailing slash? */
    else if (end && end[1] == 0) {

        /* chop off trailing slash */
        end[0] = 0;

        begin = strrchr(uri, '/');

        /* trailing slash, but no previous slash? name is the URL */
        if (!begin) {
            name = uri;
        }
        else {
            name = begin + 1;
        }

    }

    /* no trailing slash? name is everything after the slash */
    else {
        name = end + 1;
    }

    /* does uri have a file type? */
    end = strrchr(name, '.');

    if (end) {
        end[0] = 0;
    }

    return apr_punescape_url(p, name, NULL, NULL, 0);
}

static dav_prop_insert dav_displayname_insert_prop(const dav_resource *resource,
        int propid, dav_prop_insert what, apr_text_header *phdr)
{
    request_rec *r = resource->hooks->get_request_rec(resource);

    dav_displayname_config_rec *conf = ap_get_module_config(r->per_dir_config,
            &dav_displayname_module);

    apr_pool_t *p = resource->pool;
    const dav_liveprop_spec *info;
    int global_ns;

    switch (propid) {
    case DAV_PROPID_displayname:

        if (!conf->dav_displayname) {
            return DAV_PROP_INSERT_NOTDEF;
        }

        /* property allowed, handled below */
        break;
    default:
        /* ### what the heck was this property? */
        return DAV_PROP_INSERT_NOTDEF;
    }

    /* assert: value != NULL */

    /* get the information and global NS index for the property */
    global_ns = dav_get_liveprop_info(propid, &dav_displayname_liveprop_group, &info);

    /* assert: info != NULL && info->name != NULL */

    if (what == DAV_PROP_INSERT_VALUE) {

        switch (propid) {
        case DAV_PROPID_displayname: {

            apr_text_append(p, phdr, apr_psprintf(p, "<lp%d:%s>",
                    global_ns, info->name));

            apr_text_append(p, phdr, apr_pescape_entity(p, dav_displayname_render(resource), 0));

            apr_text_append(p, phdr, apr_psprintf(p, "</lp%d:%s>" DEBUG_CR,
                    global_ns, info->name));

            break;
        }
        default:
            break;
        }

    }
    else if (what == DAV_PROP_INSERT_NAME) {
        apr_text_append(p, phdr, apr_psprintf(p, "<lp%d:%s/>" DEBUG_CR, global_ns, info->name));
    }
    else {
        /* assert: what == DAV_PROP_INSERT_SUPPORTED */
        apr_text_append(p, phdr, "<D:supported-live-property D:name=\"");
        apr_text_append(p, phdr, info->name);
        apr_text_append(p, phdr, "\" D:namespace=\"");
        apr_text_append(p, phdr, dav_displayname_namespace_uris[info->ns]);
        apr_text_append(p, phdr, "\"/>" DEBUG_CR);
    }

    /* we inserted what was asked for */
    return what;
}

static int dav_displayname_is_writable(const dav_resource *resource, int propid)
{
    const dav_liveprop_spec *info;

    (void) dav_get_liveprop_info(propid, &dav_displayname_liveprop_group, &info);
    return info->is_writable;
}

static dav_error *dav_displayname_patch_validate(const dav_resource *resource,
    const apr_xml_elem *elem, int operation, void **context, int *defer_to_dead)
{
    /* We have no writable properties */
    return NULL;
}

static dav_error *dav_displayname_patch_exec(const dav_resource *resource,
    const apr_xml_elem *elem, int operation, void *context,
    dav_liveprop_rollback **rollback_ctx)
{
    /* We have no writable properties */
    return NULL;
}

static void dav_displayname_patch_commit(const dav_resource *resource, int operation,
    void *context, dav_liveprop_rollback *rollback_ctx)
{
    /* We have no writable properties */
}

static dav_error *dav_displayname_patch_rollback(const dav_resource *resource,
    int operation, void *context, dav_liveprop_rollback *rollback_ctx)
{
    /* We have no writable properties */
    return NULL;
}

static const dav_hooks_liveprop dav_hooks_liveprop_displayname =
{
    dav_displayname_insert_prop,
    dav_displayname_is_writable,
    dav_displayname_namespace_uris,
    dav_displayname_patch_validate,
    dav_displayname_patch_exec,
    dav_displayname_patch_commit,
    dav_displayname_patch_rollback
};

static int dav_displayname_find_liveprop(const dav_resource *resource,
    const char *ns_uri, const char *name, const dav_hooks_liveprop **hooks)
{
    return dav_do_find_liveprop(ns_uri, name, &dav_displayname_liveprop_group, hooks);
}


static void *create_dav_displayname_dir_config(apr_pool_t *p, char *d)
{
    dav_displayname_config_rec *conf = apr_pcalloc(p, sizeof(dav_displayname_config_rec));

    return conf;
}

static void *merge_dav_displayname_dir_config(apr_pool_t *p, void *basev, void *addv)
{
    dav_displayname_config_rec *new = (dav_displayname_config_rec *) apr_pcalloc(p,
            sizeof(dav_displayname_config_rec));
    dav_displayname_config_rec *add = (dav_displayname_config_rec *) addv;
    dav_displayname_config_rec *base = (dav_displayname_config_rec *) basev;

    new->dav_displayname = (add->dav_displayname_set == 0) ? base->dav_displayname : add->dav_displayname;
    new->dav_displayname_set = add->dav_displayname_set || base->dav_displayname_set;

    return new;
}

static const char *set_dav_displayname(cmd_parms *cmd, void *dconf, int flag)
{
    dav_displayname_config_rec *conf = dconf;

    conf->dav_displayname = flag;
    conf->dav_displayname_set = 1;

    return NULL;
}

static const command_rec dav_displayname_cmds[] =
{
    AP_INIT_FLAG("DavDisplayname",
        set_dav_displayname, NULL, RSRC_CONF | ACCESS_CONF,
        "When enabled, the DAV:displayname property will be set to the name of the resource or collection."),
    { NULL }
};


static void register_hooks(apr_pool_t *p)
{
    dav_register_liveprop_group(p, &dav_displayname_liveprop_group);
    dav_hook_find_liveprop(dav_displayname_find_liveprop, NULL, NULL, APR_HOOK_MIDDLE);
}

AP_DECLARE_MODULE(dav_displayname) =
{
    STANDARD20_MODULE_STUFF,
    create_dav_displayname_dir_config, /* dir config creater */
    merge_dav_displayname_dir_config,  /* dir merger --- default is to override */
    NULL,                              /* server config */
    NULL,                              /* merge server config */
    dav_displayname_cmds,              /* command apr_table_t */
    register_hooks                     /* register hooks */
};
