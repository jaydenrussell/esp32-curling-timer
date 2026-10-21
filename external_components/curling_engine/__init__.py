import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.esp32 as esp32
from esphome.components import time
from esphome.const import CONF_ID, CONF_TIME_ID

DEPENDENCIES = ["esp32", "time"]

CONF_STORE_PATH = "store_path"
CONF_PARTITION_SIZE = "partition_size"

# Partition label mounted at store_path; name must not collide with ESPHome's
# reserved partition names (esp32.RESERVED_PARTITION_NAMES includes "spiffs").
STORAGE_PARTITION_NAME = "storage"
DEFAULT_PARTITION_SIZE = 0x200000  # 2 MB: settings + 30 backups + slack

curling_ns = cg.esphome_ns.namespace("curling_engine")
CurlingEngineComponent = curling_ns.class_("CurlingEngineComponent", cg.Component)

def _validate_partition_size(value):
    value = cv.int_range(min=0, max=0x1000000)(value)
    if value % 0x1000 != 0:
        raise cv.Invalid("partition_size must be a multiple of 4096 (4 KB aligned)")
    return value


CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(CurlingEngineComponent),
        # Directory on SPIFFS holding pitimer_settings.yml + rolling backups.
        cv.Optional(CONF_STORE_PATH, default="/flash"): cv.string,
        # data/spiffs partition appended to the generated table (app slots
        # shrink automatically). Set to 0 if you declare your own partition.
        cv.Optional(CONF_PARTITION_SIZE, default=DEFAULT_PARTITION_SIZE): _validate_partition_size,
        cv.GenerateID(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
    }
)


async def to_code(config):
    # esp32 excludes the spiffs IDF component by default; re-enable it here.
    esp32.include_builtin_idf_component("spiffs")
    if config[CONF_PARTITION_SIZE]:
        esp32.add_partition(
            STORAGE_PARTITION_NAME, "data", "spiffs", config[CONF_PARTITION_SIZE]
        )
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_store_path(config[CONF_STORE_PATH]))
    time_ = await cg.get_variable(config[CONF_TIME_ID])
    cg.add(var.set_time(time_))
