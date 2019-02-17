# ============
# Main targets
# ============


# -------------
# Configuration
# -------------

$(eval venvpath     := .venv2)
$(eval pip          := $(venvpath)/bin/pip)
$(eval python       := $(venvpath)/bin/python)
$(eval platformio   := $(venvpath)/bin/platformio)

# Setup Python virtualenv
setup-virtualenv:
	@test -e $(python) || `command -v virtualenv` --python=python2 --no-site-packages $(venvpath)


# ----------
# PlatformIO
# ----------

install-platformio: setup-virtualenv
	@$(pip) install platformio --quiet

build-all: install-platformio
	@$(platformio) run



# Note: This are legacy build targets, the new ones are defined through `platformio.ini`.

ci-all: install-platformio
	# atmelavr
	$(platformio) ci --board=megaatmega2560 --lib="." examples/HX711_basic_example
	$(platformio) ci --board=megaatmega2560 --lib="." examples/HX711_timeout_example
	$(platformio) ci --board=megaatmega2560 --lib="." examples/HX711_full_example

	# atmelavr
	$(MAKE) ci-basic board=feather328p

	# espressif8266
	$(MAKE) ci-basic board=huzzah

	# espressif32
	$(MAKE) ci-basic board=lopy4

	# atmelsam
	$(MAKE) ci-basic board=adafruit_feather_m0
	$(MAKE) ci-basic board=adafruit_feather_m4


ci-basic:
	$(platformio) ci --board=$(board) --lib="." examples/HX711_basic_example --verbose
