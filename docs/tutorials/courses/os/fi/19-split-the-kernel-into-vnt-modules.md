# Jaa kernel VNT-moduuleihin

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Kun kernel kasvaa, moduulit helpottavat vastuiden erottamista. VNTOSissa esimerkiksi muistinhallinta, keskeytykset ja komentotulkki ovat erillisissä tiedostoissa. Tavoite ei ole luoda mahdollisimman montaa tiedostoa, vaan antaa jokaiselle osalle selkeä tehtävä.

Tutki `os/kernel.vnt`, `os/interrupts.vnt`, `os/mm/phys.vnt` ja `os/kernel/shell.vnt`. Selvitä, miten build löytää moduulit ja miten niiden funktioita kutsutaan. Kirjaa myös, kuka alustaa yhteisen tilan ja kuka saa muuttaa sitä.

**Pohdi:** Miksi moduulien vastuut ja yhteisen tilan omistajuus pitää määritellä?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Mihin jatkaa kurssin jälkeen?](./18-next-steps.md) · [Seuraava oppitunti: Keskeytykset, poikkeukset ja IDT →](./20-interrupts-and-the-idt.md)
