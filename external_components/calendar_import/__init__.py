import esphome.codegen as cg
from esphome import pins
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_URL

DEPENDENCIES = ["http_request"]
CODEOWNERS = ["@jaydenrussell"]
DOMAIN = "calendar_import"

calendar_import_ns = cg.esphome_ns.namespace("calendar_import")
CalendarImport = calendar_import_ns.class_("CalendarImport", cg.Component)

CONF_ICS_URL = "ics_url"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(CalendarImport),
        cv.Required(CONF_ICS_URL): cv.url,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_url(config[CONF_ICS_URL]))
    await cg.register_component(var, config)
