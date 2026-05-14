# Soil Moisture Sensor Technologies

We need a seedling starter tray soil moisture sensor less than 20mm wide.
According to the user fom a seed nursery it is not good that water flows from the hole in the tray.

## The "Representative Cell" Strategy

Since we cant sense water under the try we need to sense it in the soil inside the tray. We can prepare a special big cell to place the sensor but it wont be exactly the same as the other cells. 
Since you can't put a sensor in every single one of the 50+ cells, place your sensor in a middle-row cell.
The edges of the tray always dry out first due to airflow.
The center stays damp the longest.
Monitoring the center ensures you don't drown the majority of your plants while trying to keep the edges wet.

## Conclusion (7/04/2026)

The main technologies are: capacitive, resistive, the problem with resistive is that the probes corrode. The best option is to use capacitive sensors.

There is no sensor that meets the requirements. The closest are 20mm or more.
We need to redesign and produce a custom sensor. this an achievable goal because there are many examples of similar sensors that can be used as a reference.

## 1. What a “soil moisture sensor” measures

METER Group states that the expression *soil moisture sensor* can refer to two different measurands: **soil water content** or **soil water potential / suction / tension**. The same source groups the main measurement methods into **resistance**, **dielectric permittivity**—including **TDR, FDR, and capacitance**—**thermal conductivity**, and **thermalized neutrons**.[^1]

## 2. Main sensing technologies

### 2.1 Resistive two-electrode sensors

DFRobot describes resistive soil-moisture sensors as devices that use **two probes** and estimate moisture from the **resistance** measured through the soil. Its explanation states that wetter soil conducts electricity more easily, so the resistance-based reading changes with moisture.[^2]

METER Group describes resistance probes as sensors that create a voltage difference across two electrodes, allow a small current to flow between them, and output a value of **resistance or electrical conductivity**.[^1]

### 2.2 Gypsum blocks and granular-matrix sensors

A University of California / IRROMETER extension document states that **gypsum blocks** monitor soil moisture **indirectly** by measuring the **electrical resistance between two electrodes** attached to a small gypsum block buried in the soil.[^3]

IRROMETER states that the **WATERMARK 200SS** is a **granular matrix** sensor that measures **electrical resistance inside a granular matrix** to determine **soil water tension**, and that the sensor presents itself electrically as a **variable resistor** whose value changes with soil water tension.[^4]

### 2.3 Capacitive sensors

ScioSense states that a capacitive soil-moisture probe estimates soil water content by tracking the **capacitance change of the probe** as the **dielectric permittivity** of the surrounding soil changes.[^5]

DFRobot states that the SEN0193 measures soil moisture by **capacitive sensing rather than resistive sensing**.[^6]

### 2.4 FDR (Frequency Domain Reflectometry)

Proximal Soil Sensing states that **FDR** and capacitance probes measure the soil **dielectric constant** using plates or rods inserted in the soil, and that in FDR the dielectric constant is determined by applying an **oscillating charge** to the circuit and measuring its **resonant frequency**.[^7]

### 2.5 TDR (Time Domain Reflectometry)

ICT International states that **TDR** calculates **volumetric soil moisture content** by measuring dielectric properties. The same source states that the moisture value measured by TDR is the **average along the length of the waveguides**.[^8]

METER Group lists the following explicit points for **TDR probes**: continuous measurements can be collected with a data logger; the method can be accurate with soil-specific calibration; it is described as insensitive to salinity until the signal disappears; and its listed limitations are greater complexity, trench-style installation, failure in high salinity, and relatively high power use.[^1]

### 2.6 Tensiometers

Soilmoisture Equipment Corp. states that tensiometers measure **soil water tension** or **matric potential**, which indicates how strongly water is held in the soil and therefore how available it is to plants.[^9]

IRROMETER states that its tensiometers provide a **direct, physical measurement** of the same tension plant roots experience, and that they require **no electronics, batteries, or calibrations** to operate. IRROMETER also states that proper operation requires the instrument to be **filled with water** and **de-aired**.[^10]

## 3. Commercial sensors found on MercadoLibre or AliExpress

### 3.1 YL-69 / YL-38

**Marketplace availability.** MercadoLibre Argentina currently shows YL-69 listings and product pages for this module family.[^11]

**Declared sensing principle.** Marketplace and vendor pages for the YL-69 / YL-38 describe it as a resistive sensor that measures soil moisture through **two electrodes** and a conditioning board with an **LM393** comparator.[^12]

**Published detector size.** A current product page lists the dimensions as **sensor probe: 60 mm × 20 mm** and **PCB module: 30 mm × 15 mm**.[^13]

### 3.2 Capacitive Soil Moisture Sensor v1.2 / DFRobot SEN0193-type

**Marketplace availability.** MercadoLibre Argentina currently shows listings for the analog capacitive v1.2 sensor, and AliExpress also shows current listings for the same low-cost capacitive board family.[^14][^15]

**Declared sensing principle.** The official DFRobot SEN0193 datasheet states that the sensor measures soil moisture by **capacitive sensing rather than resistive sensing**.[^6]

**Published detector size.** The official DFRobot/SEN0193 specification gives **3.86 × 0.905 inches (L × W)**, which corresponds to approximately **98 mm × 23 mm**.[^6]

### 3.3 ECOWITT WH51

**Marketplace availability.** MercadoLibre Argentina currently shows WH51 listings.[^16]

**Declared sensing principle.** The current Ecowitt manual states that the WH51 uses **Advanced Capacitive Sensing** and the **principle of soil dielectric permittivity** to measure moisture levels.[^17]

**Published detector size.** The current Ecowitt manual’s size figure shows **148 mm total height**, **86 mm body length**, **77 mm probe length**, **20 mm probe width**, and **1.6 mm probe thickness**.[^17]

## 4. Advantages and disadvantages of the commercial sensors above

### 4.1 YL-69 / YL-38

**Advantages explicitly stated by cited sources.** Product pages describe the YL-69 / YL-38 as a simple resistive sensor with **analog and digital output**, an **LM393** comparator stage, and broad compatibility with Arduino-style platforms. Sellers also describe it as usable for **automatic irrigation** projects.[^12][^18]

**Disadvantages or limitations explicitly stated by cited sources.** A current product page states that the **immersed electrode life** is **3–6 months**.[^18]

### 4.2 Capacitive Soil Moisture Sensor v1.2 / SEN0193-type

**Advantages explicitly stated by cited sources.** DFRobot and Arduino state that this capacitive board uses **corrosion-resistant material**, is intended to provide **long service life**, and includes an onboard regulator for **3.3–5.5 V** systems.[^6][^19]

**Disadvantages or limitations explicitly stated by cited sources.** DFRobot states that the upper electronics are **not waterproof**, that moisture exposure should not exceed the marked red line, that output depends on **probe insertion depth** and **soil packing**, and that a **Raspberry Pi requires an ADC**.[^20]

### 4.3 ECOWITT WH51

**Advantages explicitly stated by cited sources.** Ecowitt states that the WH51 provides **manual calibration**, a **trend-based mode**, **IP66** waterproofing, **wireless RF transmission**, support for up to **16 channels** with compatible gateways/consoles, and wireless range up to **100 m** under standard conditions, with an optional antenna extending the range to **200 m in open fields**.[^17]

**Disadvantages or limitations explicitly stated by cited sources.** Ecowitt’s store page states that the WH51 has **no display** and **needs a compatible gateway** to complete Wi‑Fi setup in the app. The manual states that accurate readings require the sensor to be **inserted totally into the soil**, that the probe should be installed in a **pre-dug hole** rather than forced into hard soil, and that calibration is **saved on the gateway side**.[^17][^21]

## 5. Calibration of capacitive soil-moisture sensors

### 5.1 Calibration is repeatedly described as soil-specific

SparkFun states that its capacitive sensor should be calibrated for **different soil compositions** to obtain reliable and usable data. Its hookup guide says to record the minimum value in a **completely dry** sample of the same soil and the maximum value in a **completely saturated** sample of that soil.[^22][^23]

Ecowitt states that the WH51 **needs to be calibrated to suit your soil type** and recommends **manual calibration** for scientific and industrial applications requiring high accuracy.[^17]

DFRobot states that the SEN0193 output is affected by **probe insertion depth** and by **how tight the soil is packed** around the probe, and its example procedure uses a **dry boundary** and a **wet boundary** for calibration values in code.[^20]

### 5.2 Calibration procedures explicitly described in application notes and papers

ScioSense describes a calibration workflow based on **reference measurements** and firmware-side mapping between raw measured values and a normalized moisture scale.[^24]

The 2021 Sensors paper linked by the user through PMCID **PMC8399650** states that its designed capacitive sensor was calibrated on **clay-textured soils** using moisture contents of **0, 10, 20, 30, 40, and 50%**; during calibration, the sensor was inserted at **6 cm depth**, and reference moisture was determined from samples oven dried at **105 °C for 24 h**.[^25]

The same paper states that higher soil moisture was associated with **decreased sensor output voltage**, reports an average **R² of 0.967** and **RMSE of 0.014**, and states that more research is required to test and calibrate the designed sensor with other soils such as **loamy and sandy** soils.[^25]

### 5.3 Explicit calibration findings from other published studies

A 2020 Sensors paper states that low-cost capacitive sensors could match the performance of the secondary standard sensor **after soil-specific calibration**, and that the capacitive sensors in that study responded reasonably to **temperature and salinity** variations.[^26]

A 2024 Sensors paper states that in a **biochar-amended sandy loam** with fertilizers and multiple salinity levels, the best fit for soil-water-content calibration was obtained using **polynomial equations** with values greater than **0.98** for each case. The same paper states that **biochar and salinity** increased the capacitance detected by the sensors at low frequencies, that calibration curves deviated by up to **30%** from the control sandy loam soil, and that **specific and continuous calibrations** were required.[^27]

## 6. Extra section: capacitive sensor design

### 6.1 Core design principle

ScioSense states that a capacitive soil-moisture system uses a **two-electrode probe**, and that the effective capacitance changes as soil permittivity changes. In its application note, the capacitance is described with the approximation **C ≈ ε0 · εeff · G**, where **G** is the geometry factor of the probe.[^5]

The same note states that the **Single Floating** configuration is recommended for **long leads** and **high-impedance probes**, and that an **external reference capacitor** is recommended because the internal reference range may be insufficient across the full moisture range.[^24]

### 6.2 Signal-chain design examples from published sources

The 2021 Sensors paper states that its designed capacitive sensor consisted of **three main circuits**: an **oscillation circuit**, a **low-pass filter**, and a **half-wave rectifier / peak detector**. The paper states that the oscillator used a **555 IC** in astable mode at **430 kHz** with an approximate **55% duty cycle**.[^25]

DFRobot’s waterproof capacitive sensor page states that the SEN0308 uses **advanced capacitive sensing technology**, provides an **analog output**, is rated **IP65**, and has published dimensions of **175 × 30 mm**.[^28]

### 6.3 Design details from the user-provided Maakbaas reference

Maakbaas states that the sensor uses a **large metal pad** in the soil to perform a capacitive measurement. The same page states that the design places a **ground plane around the sensor** to shield it from disturbances, but **no ground on the bottom layer** in order to keep sensitivity as high as possible.[^29]

Maakbaas also states that the ESP32’s **touch GPIO inputs** were used because they already perform capacitive measurement on a metal pad, which made additional sensor circuitry unnecessary in that design.[^29]

### 6.4 Design details from the user-provided `theopensourcerer` GitHub repository

The `esp8266-cap-soil-sensor` repository states that its fundamental principle is to measure the **time it takes for the capacitor (the probe) to charge**, and that there is **no direct electrical contact** between the soil and the probe because the probe is **insulated**.[^30]

The same repository states that it uses a **Schmitt Trigger Inverter** to square the probe oscillations, and that the ESP8266 was considered capable of reading the frequency directly **if it can be kept below about 150 kHz**.[^30]

The repository also states that the **schematic and PCB are designed in KiCad**, that the case is designed in **OpenSCAD**, and that the PCB probe footprint was modified from an earlier layout and made **longer** for larger pots.[^30]

## 7. Open-hardware design references (selected)

This section only lists repositories or project pages that explicitly state the presence of hardware design files, open-hardware status, or concrete PCB/case resources.

### 7.1 SparkFun capacitive Qwiic sensor

SparkFun’s capacitive soil-moisture repository states that the sensor measures moisture using a **capacitive plate in the board** together with a **CY8CMBR3102 Capsense controller**, that it has **no exposed metal leads**, and that code packages are available for **Arduino** and **MicroPython**.[^23]

The repository also states that it includes **documentation**, **datasheets**, **KiCad hardware files**, and **production files**, and explicitly says: **“This product is open source!”**.[^23]

SparkFun’s documentation resources page links a **board dimensions** drawing, and the published dimensions image shows an overall board width of **0.6 in** and an overall length of **3 in**.[^31]

### 7.2 SparkFun resistive soil-moisture sensor

SparkFun’s older resistive `Soil_Moisture_Sensor` repository states that it contains **Eagle design files** and **production panel files**, and also explicitly says: **“This product is open source!”**.[^32]

SparkFun’s product page states that the board operates with **two large exposed pads** acting together as a **variable resistor**, and that the sensing leads use an **ENIG finish** intended to extend board life in wet environments.[^33]

### 7.3 `theopensourcerer/esp8266-cap-soil-sensor`

This repository documents a **battery-powered ESP8266 capacitive soil moisture sensor** and explicitly includes **KiCad** PCB design and an **OpenSCAD** case. It also explains the design choice of an **insulated probe** and a **Schmitt-trigger-based frequency readout** path.[^30]

### 7.4 `RonMcKay/capacitive-soil-moisture-sensor`

This repository states that the design is a **capacitive soil moisture sensor** built around a **standalone ATmega328P**, uses an **NRF24L01** radio, runs on **2 AAA batteries**, has **8 µA** sleeping current, places **all SMD components on one side of the PCB**, and includes a **3D printable case**. The repository also states that the **PCB design is done in KiCad 6**.[^34]

### 7.5 `rbaron/b-parasite`

The `b-parasite` repository states that it is an **open source** soil moisture plus ambient temperature/humidity/light sensor. It lists a **capacitive soil moisture sensor**, support for **nRF52840** and **nRF52833** modules, a **KiCad** directory for the PCB, a **data** directory for testing and calibration, and a **3D printable case** directory.[^35]

### 7.6 `bahildebrand/soil-moisture-pcb`

This repository states that it is a **capacitive soil moisture sensor routed in KiCad**, uses a **Fastnel NRF52840 module**, and is powered by a **3.0 V coin cell battery**.[^36]

## 8. Concise closing note

Across the cited sources, the most commonly documented low-cost market options are **resistive** modules such as the **YL-69** and **capacitive** PCB probes such as the **SEN0193-type v1.2**. The added references strengthen two parts of the report in particular: **capacitive calibration** and **capacitive/open-hardware design**. The user-provided DFRobot, Maakbaas, PMCID/MDPI, and GitHub references contribute explicit information about **dry/wet or soil-specific calibration**, **oscillator/filter/rectifier signal chains**, **shielding and pad layout**, **insulated probes**, and the availability of **KiCad / Eagle / production files / case files** in open repositories.[^6][^22][^25][^28][^29][^30][^32]

## References

[^1]: METER Group, *Soil moisture sensors—How they work. Why some are not research grade*. https://metergroup.com/measurement-insights/soil-moisture-sensors-how-they-work-why-some-are-not-research-grade/
[^2]: DFRobot, *Capacitive v/s Resistive Soil Moisture Sensor*. https://www.dfrobot.com/blog-1156.html
[^3]: UC Davis / IRROMETER, *Drought Management: Gypsum Blocks*. https://www.irrometer.com/pdf/ext/DROUGHT_WEB_RESIS_BLOCKS-UC%20DAVIS_Drought_Management.pdf
[^4]: IRROMETER, *WATERMARK 200SS Sensors*. https://www.irrometer.com/200ss.html
[^5]: ScioSense, *Capacitive Soil Moisture Measurement with PCAP04 Evaluation Kit*. https://www.sciosense.com/wp-content/uploads/2026/02/PCAP04-Applicatio-Note-Capacitive-Soil-Moisture-Measurement-with-PCAP04-Evaluation-Board.pdf
[^6]: DFRobot, *Capacitive Soil Moisture Sensor SKU:SEN0193* datasheet / web PDF. https://media.digikey.com/pdf/Data%20Sheets/DFRobot%20PDFs/SEN0193_Web.pdf
[^7]: Proximal Soil Sensing, *Frequency Domain Reflectometry (FDR) / Capacitance*. https://www.proximalsoilsensing.org/wp-content/uploads/techniques/fdr.html
[^8]: ICT International, *Soil Moisture Measurement: Time Domain Reflectometry (TDR)*. https://ictinternational.com/knowledge-base/soil-moisture-measurement-time-domain-reflectometry-tdr/
[^9]: Soilmoisture Equipment Corp., *Tensiometers*. https://soilmoisture.com/product-line/soil-sensors/tensiometers/
[^10]: IRROMETER, *SR Series / LT Series / Reference Handbook* pages and manuals. https://www.irrometer.com/pdf/105.pdf ; https://www.irrometer.com/pdf/109.pdf ; https://www.irrometer.com/pdf/714.pdf
[^11]: MercadoLibre Argentina, YL-69 product/listing pages. https://www.mercadolibre.com.ar/5pz-sensor-de-humedad-de-tierra-yl-69-compatible-con-arduino/p/MLA2017750825 ; https://listado.mercadolibre.com.ar/sensor-yl-69
[^12]: Current vendor pages describing YL-69 / YL-38 construction and outputs. https://www.taloselectronics.com/products/sensor-de-humedad-del-suelo-yl38-y-yl69 ; https://epyelectronica.com/tienda/sensores-y-modulos/sensores-sensores-y-modulos/sensor-de-humedad-de-suelo-yl-69/
[^13]: Electrónica Sigma Guatemala, *Sensor de Humedad de Suelo YL-69 YL-38*. https://electronicasigma.com.gt/producto/sensor-de-humedad-de-suelo-yl-69-yl-38/
[^14]: MercadoLibre Argentina, capacitive v1.2 listings. https://articulo.mercadolibre.com.ar/MLA-875205667-sensor-capacitivo-analogico-de-humedad-del-suelo-v12-_JM ; https://www.mercadolibre.com.ar/sensor-capacitivo-analogico-humedad-suelo-v12/p/MLA37347159
[^15]: AliExpress, capacitive soil moisture sensor board family listing. https://www.aliexpress.us/item/3256809424577493.html
[^16]: MercadoLibre Argentina, WH51 listing pages. https://articulo.mercadolibre.com.ar/MLA-1482269085-ecowitt-wh51-sensor-de-humedad-del-suelo-probador-de-humedad-_JM ; https://www.mercadolibre.com.ar/sensor-de-humedad-del-suelo-ecowitt-wh51l-pantalla-lcd/up/MLAU3337816034
[^17]: Ecowitt, *WH51 Manual*. https://oss.ecowitt.net/uploads/20251226/WH51Manual.pdf
[^18]: Makers Chile, *Sensor Humedad Suelo Yl-38 Yl-69*. https://makerschile.cl/producto/sensor-humedad-suelo-yl-38-yl-69/
[^19]: Arduino Official Store, *Gravity: Analog Capacitive Soil Moisture Sensor – Corrosion Resistant*. https://store.arduino.cc/products/gravity-analog-capacitive-soil-moisture-sensor-corrosion-resistant
[^20]: DFRobot SEN0193 calibration section and notes in official web PDF. https://media.digikey.com/pdf/Data%20Sheets/DFRobot%20PDFs/SEN0193_Web.pdf
[^21]: Ecowitt official shop page, *WH51 Wireless Soil Moisture Sensor*. https://shop.ecowitt.com/es/products/wh51
[^22]: SparkFun, *SparkFun Qwiic Soil Moisture Sensor (Capacitive) Hookup Guide*. https://docs.sparkfun.com/SparkFun_Capacitive_Soil_Moisture_Sensor_CY8CMBR3102/single_page/
[^23]: GitHub, sparkfun/SparkFun_Capacitive_Soil_Moisture_Sensor_CY8CMBR3102. https://github.com/sparkfun/SparkFun_Capacitive_Soil_Moisture_Sensor_CY8CMBR3102
[^24]: ScioSense, calibration and reference-measurement sections in *Capacitive Soil Moisture Measurement with PCAP04 Evaluation Kit*. https://www.sciosense.com/wp-content/uploads/2026/02/PCAP04-Applicatio-Note-Capacitive-Soil-Moisture-Measurement-with-PCAP04-Evaluation-Board.pdf
[^25]: Okasha et al., *Designing Low-Cost Capacitive-Based Soil Moisture Sensor and Smart Monitoring Unit Operated by Solar Cells for Greenhouse Irrigation Management*, Sensors 2021, 21(16), 5387. MDPI page: https://www.mdpi.com/1424-8220/21/16/5387 ; user-provided PMCID link: https://pmc.ncbi.nlm.nih.gov/articles/PMC8399650/
[^26]: Blonquist et al., *Laboratory Calibration and Performance Evaluation of Low-Cost Capacitive and Very Low-Cost Resistive Soil Moisture Sensors*, Sensors 2020, 20(2), 363. https://www.mdpi.com/1424-8220/20/2/363
[^27]: *Calibration of Low-Cost Moisture Sensors in a Biochar-Amended Sandy Loam Soil with Different Salinity Levels*, Sensors 2024, 24(18), 5958. https://www.mdpi.com/1424-8220/24/18/5958
[^28]: DFRobot, *Gravity: IP65 Capacitive Soil Moisture Sensor for Arduino / ESP32 / Raspberry Pi* (SKU:SEN0308). https://www.dfrobot.com/product-2054.html
[^29]: Maakbaas, *Designing a soil moisture sensor using the ESP32 – hardware design*. https://maakbaas.com/esp32-soil-moisture-sensor/logs/hardware-design/
[^30]: GitHub, theopensourcerer/esp8266-cap-soil-sensor. https://github.com/theopensourcerer/esp8266-cap-soil-sensor/tree/master
[^31]: SparkFun, board-dimensions drawing for the Qwiic capacitive soil-moisture sensor. https://docs.sparkfun.com/SparkFun_Capacitive_Soil_Moisture_Sensor_CY8CMBR3102/assets/board_files/SparkFun_Qwiic_Soil_Moisture_Sensor_Capacitive.jpg
[^32]: GitHub, sparkfun/Soil_Moisture_Sensor. https://github.com/sparkfun/Soil_Moisture_Sensor/tree/master
[^33]: SparkFun, *SparkFun Soil Moisture Sensor* product page. https://www.sparkfun.com/sparkfun-soil-moisture-sensor.html
[^34]: GitHub, RonMcKay/capacitive-soil-moisture-sensor. https://github.com/RonMcKay/capacitive-soil-moisture-sensor
[^35]: GitHub, rbaron/b-parasite. https://github.com/rbaron/b-parasite
[^36]: GitHub, bahildebrand/soil-moisture-pcb. https://github.com/bahildebrand/soil-moisture-pcb