/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * SDMMC disk driver using zephyr SD subsystem
 */
#define DT_DRV_COMPAT zephyr_sdmmc_disk

#include <zephyr/sd/sdmmc.h>
#include <zephyr/drivers/disk.h>


enum sd_status {
	SD_UNINIT,
	SD_ERROR,
	SD_OK,
};

struct sdmmc_config {
	const struct device *host_controller;
	const char *name;
};

struct sdmmc_data {
	struct sd_card card;
	enum sd_status status;
	struct disk_info disk_info;
};


static int disk_sdmmc_access_init(struct disk_info *disk)
{
	const struct device *dev = disk->dev;
	const struct sdmmc_config *cfg = dev->config;
	struct sdmmc_data *data = dev->data;
	int ret;

	if (!sd_is_card_present(cfg->host_controller)) {
		return DISK_STATUS_NOMEDIA;
	}

	ret = sd_init(cfg->host_controller, &data->card);
	if (ret) {
		data->status = SD_ERROR;
		return ret;
	}
	data->status = SD_OK;
	return 0;
}

static int disk_sdmmc_access_status(struct disk_info *disk)
{
	const struct device *dev = disk->dev;
	const struct sdmmc_config *cfg = dev->config;
	struct sdmmc_data *data = dev->data;

	if (!sd_is_card_present(cfg->host_controller)) {
		return DISK_STATUS_NOMEDIA;
	}
	if (data->status == SD_OK) {
		return DISK_STATUS_OK;
	} else {
		return DISK_STATUS_UNINIT;
	}
}

static int disk_sdmmc_access_read(struct disk_info *disk, uint8_t *buf,
				 uint32_t sector, uint32_t count)
{
	const struct device *dev = disk->dev;
	struct sdmmc_data *data = dev->data;

	return sdmmc_read_blocks(&data->card, buf, sector, count);
}

static int disk_sdmmc_access_write(struct disk_info *disk, const uint8_t *buf,
				 uint32_t sector, uint32_t count)
{
	const struct device *dev = disk->dev;
	struct sdmmc_data *data = dev->data;

	return sdmmc_write_blocks(&data->card, buf, sector, count);
}

static int disk_sdmmc_access_ioctl(struct disk_info *disk, uint8_t cmd, void *buf)
{
	const struct device *dev = disk->dev;
	struct sdmmc_data *data = dev->data;

	switch (cmd) {
	case DISK_IOCTL_CTRL_INIT:
		return disk_sdmmc_access_init(disk);
	case DISK_IOCTL_CTRL_DEINIT:
		/* Card will be uninitialized after DEINIT */
		data->status = SD_UNINIT;
		return sdmmc_ioctl(&data->card, DISK_IOCTL_CTRL_DEINIT, NULL);
	default:
		return sdmmc_ioctl(&data->card, cmd, buf);
	}

	return 0;
}

static const struct disk_operations sdmmc_disk_ops = {
	.init = disk_sdmmc_access_init,
	.status = disk_sdmmc_access_status,
	.read = disk_sdmmc_access_read,
	.write = disk_sdmmc_access_write,
	.ioctl = disk_sdmmc_access_ioctl,
};

static int disk_sdmmc_init(const struct device *dev)
{
	const struct sdmmc_config *config = dev->config;
	struct sdmmc_data *data = dev->data;

	data->status = SD_UNINIT;
	data->disk_info.name = config->name;
	data->disk_info.ops = &sdmmc_disk_ops;
	data->disk_info.dev = dev;

	return disk_access_register(&data->disk_info);
}

#define DISK_ACCESS_SDMMC_INIT(n)						\
	static const struct sdmmc_config sdmmc_config_##n = {			\
		.host_controller = DEVICE_DT_GET(DT_INST_PARENT(n)),		\
		.name = DT_INST_PROP(n, disk_name),                             \
	};									\
										\
	static struct sdmmc_data sdmmc_data_##n;                                \
										\
	DEVICE_DT_INST_DEFINE(n,						\
			&disk_sdmmc_init,					\
			NULL,							\
			&sdmmc_data_##n,					\
			&sdmmc_config_##n,					\
			POST_KERNEL,						\
			CONFIG_SD_INIT_PRIORITY,				\
			NULL);

DT_INST_FOREACH_STATUS_OKAY(DISK_ACCESS_SDMMC_INIT)
