# SMP-turvallisuus: synkronointi ennen yhteistä muistia

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Kun kaksi CPU:ta muuttaa samaa muistia, niiden toiminnot voivat mennä päällekkäin. Tätä kilpailutilannetta kutsutaan race conditioniksi. Esimerkiksi kaksi CPU:ta voi nähdä saman sivukehyksen vapaana ja antaa sen molemmat omalle kutsujalleen.

VNTOSin `os/mm/phys.vnt` muuttaa bittikarttaa ja vapaiden kehysten laskuria. Nämä muutokset pitää suojata yhtenä kokonaisuutena ennen moniydinkäyttöä. Atomisen operaation täytyy olla oikein määritelty sekä CPU:lle että kääntäjälle. Satunnainen inline-assembly-teksti ei yksin takaa tätä.

VNTOSin IPI-testi ei tee muistivaraajasta SMP-turvallista. Tarvitaan atominen primitiivi, VNT-kielinen lukko, kääntäjätestit ja moniydintesti.

**Pohdi:** Selitä kilpailutilanne omin sanoin. Miksi IPI-testin onnistuminen ei riitä?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: APIC IPI -viestit](./28-apic-ipi-service.md) · [Kurssi valmis — takaisin sisällysluetteloon](./README.md)
