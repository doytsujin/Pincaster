
#include "common.h"
#include "app_config.h"
#include "parser.h"

int parse_config(const char * const file)
{
    char *cfg_server_ip = NULL;
    char *cfg_server_port = NULL;
    char *cfg_timeout_s = NULL;    
    char *cfg_nb_workers_s = NULL;
    char *cfg_max_queued_replies_s = NULL;
    char *cfg_default_layer_type_s = NULL;
    char *cfg_default_accuracy_s = NULL;
    char *cfg_bucket_size_s = NULL;
    char *cfg_dimension_accuracy_s = NULL;
    char *cfg_db_log_file_name = NULL;
    char *cfg_journal_buffer_size_s = NULL;    
    char *cfg_fsync_period_s = NULL;
    int ret = 0;
    ConfigKeywords config_keywords[] = {
        { "ServerIP",          &cfg_server_ip },
        { "ServerPort",        &cfg_server_port },
        { "Timeout",           &cfg_timeout_s },        
        { "Workers",           &cfg_nb_workers_s },
        { "MaxQueuedReplies",  &cfg_max_queued_replies_s },
        { "DefaultLayerType",  &cfg_default_layer_type_s },
        { "Accuracy",          &cfg_default_accuracy_s },
        { "BucketSize",        &cfg_bucket_size_s },
        { "DimensionAccuracy", &cfg_dimension_accuracy_s },
        { "DBFileName",        &cfg_db_log_file_name },
        { "JournalBufferSize", &cfg_journal_buffer_size_s },
        { "FsyncPeriod",       &cfg_fsync_period_s },
        { NULL,                NULL }
    };
    app_context = (AppContext) {
        .server_ip = NULL,
        .server_port = strdup(DEFAULT_SERVER_PORT),
        .timeout = DEFAULT_CLIENT_TIMEOUT,
        .nb_workers = NB_WORKERS,
        .max_queued_replies = MAX_QUEUED_REPLIES,
        .default_layer_type = DEFAULT_LAYER_TYPE,
        .default_accuracy = DEFAULT_ACCURACY,
        .bucket_size = BUCKET_SIZE,            
        .dimension_accuracy = DEFAULT_DIMENSION_ACCURACY
    };
    if (app_context.server_port == NULL) {
        _exit(1);
    }
    if (generic_parser(file, config_keywords) != 0) {
        return -1;
    }
    char *endptr;
    if (cfg_server_ip != NULL) {
        if (*cfg_server_ip == 0) {
            ret = -1;
        } else {
            free(app_context.server_ip);
            app_context.server_ip = cfg_server_ip;
        }
    }
    if (cfg_server_port != NULL) {
        if (*cfg_server_port == 0) {
            ret = -1;
        } else {
            free(app_context.server_port);            
            app_context.server_port = cfg_server_port;
        }
    }
    if (cfg_timeout_s != NULL) {
        app_context.timeout = (int) strtol(cfg_timeout_s, &endptr, 10);
        if (endptr == NULL || endptr == cfg_timeout_s ||
            app_context.timeout <= 0) {
            ret = -1;
        }
    }
    if (cfg_nb_workers_s != NULL) {
        app_context.nb_workers = strtoul(cfg_nb_workers_s, &endptr, 10);
        if (endptr == NULL || endptr == cfg_nb_workers_s ||
            app_context.nb_workers <= 0U) {
            ret = -1;
        }
    }
    if (cfg_max_queued_replies_s != NULL) {
        app_context.max_queued_replies =
            (size_t) strtoull(cfg_max_queued_replies_s, &endptr, 10);
        if (endptr == NULL || endptr == cfg_max_queued_replies_s ||
            app_context.max_queued_replies <= (size_t) 0U) {
            ret = -1;
        }
    }
    if (cfg_default_layer_type_s != NULL) {
        if (strcasecmp(cfg_default_layer_type_s, "flat") == 0) {
            app_context.default_layer_type = LAYER_TYPE_FLAT;
        } else if (strcasecmp(cfg_default_layer_type_s, "flatwrap") == 0) {
            app_context.default_layer_type = LAYER_TYPE_FLATWRAP;            
        } else if (strcasecmp(cfg_default_layer_type_s, "spherical") == 0) {
            app_context.default_layer_type = LAYER_TYPE_SPHERICAL;            
        } else if (strcasecmp(cfg_default_layer_type_s, "geoidal") == 0) {
            app_context.default_layer_type = LAYER_TYPE_GEOIDAL;
        } else {
            ret = -1;
        }
    }
    if (cfg_default_accuracy_s != NULL) {
        if (strcasecmp(cfg_default_accuracy_s, "haversine") == 0) {
            app_context.default_accuracy = ACCURACY_HS;
        } else if (strcasecmp(cfg_default_accuracy_s, "greatcirle") == 0) {
            app_context.default_accuracy = ACCURACY_GC;
        } else if (strcasecmp(cfg_default_accuracy_s, "fast") == 0) {
            app_context.default_accuracy = ACCURACY_FAST;
        } else if (strcasecmp(cfg_default_accuracy_s, "romboid") == 0) {
            app_context.default_accuracy = ACCURACY_ROMBOID;
        } else {
            ret = -1;
        }
    }
    if (cfg_bucket_size_s != NULL) {
        app_context.bucket_size =
            (size_t) strtoull(cfg_bucket_size_s, &endptr, 10);
        if (endptr == NULL || endptr == cfg_bucket_size_s ||
            app_context.bucket_size <= (size_t) 0U) {
            ret = -1;
        }
    }
    if (cfg_dimension_accuracy_s != NULL) {
        app_context.dimension_accuracy =
            (Dimension) strtod(cfg_dimension_accuracy_s, &endptr);
        if (endptr == NULL || endptr == cfg_dimension_accuracy_s ||
            app_context.dimension_accuracy <= (Dimension) 0.0) {
            ret = -1;
        }
    }
    if (cfg_db_log_file_name != NULL) {
        if (*cfg_db_log_file_name == 0) {
            ret = -1;
        } else {
            app_context.db_log.db_log_file_name = cfg_db_log_file_name;
        }
    }
    if (cfg_journal_buffer_size_s != NULL) {
        app_context.db_log.journal_buffer_size =
            (size_t) strtoull(cfg_journal_buffer_size_s, &endptr, 10);
        if (endptr == NULL || endptr == cfg_journal_buffer_size_s ||
            app_context.db_log.journal_buffer_size <= (size_t) 0U) {
            ret = -1;
        }
    }
    if (cfg_fsync_period_s != NULL) {
        app_context.db_log.fsync_period =
            (int) strtol(cfg_fsync_period_s, &endptr, 10);
        if (endptr == NULL || endptr == cfg_fsync_period_s ||
            app_context.db_log.fsync_period < -1) {
            ret = -1;
        }
    }
    free(cfg_timeout_s);    
    free(cfg_nb_workers_s);
    free(cfg_max_queued_replies_s);
    free(cfg_default_layer_type_s);
    free(cfg_default_accuracy_s);
    free(cfg_bucket_size_s);
    free(cfg_dimension_accuracy_s);
    free(cfg_journal_buffer_size_s);
    free(cfg_fsync_period_s);    
    
    return ret;
}

int check_sys_config(void)
{
    int ret = 0;
    
#ifdef __linux__
    FILE *fp;
    char tmp[100];
        
    if ((fp = fopen("/proc/sys/vm/overcommit_memory", "r")) != NULL) {
        if (fgets(tmp, sizeof tmp, fp) != NULL) {
            if (atoi(tmp) <= 0) {
                fputs("* Please add vm.overcommit_memory=1 "
                      "to /etc/sysctl.conf\n", stderr);
                ret = -1;
            }
        }
        fclose(fp);
    }
    if ((fp = fopen("/proc/sys/net/ipv4/tcp_tw_reuse", "r")) != NULL) {
        if (fgets(tmp, sizeof tmp, fp) != NULL) {
            if (atoi(tmp) <= 0) {
                fputs("* Please add net.ipv4.tcp_tw_reuse=1 "
                      "to /etc/sysctl.conf\n", stderr);
            }
        }
        fclose(fp);
    }
#endif
    return ret;
}

void free_config(void)
{
    free(app_context.server_ip);
    app_context.server_ip = NULL;
    free(app_context.server_port);
    app_context.server_port = NULL;
    free(app_context.db_log.db_log_file_name);    
    app_context.db_log.db_log_file_name = NULL;
}
