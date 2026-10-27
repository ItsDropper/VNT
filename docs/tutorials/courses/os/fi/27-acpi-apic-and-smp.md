# ACPI, APIC ja usean CPU:n käynnistys

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

ACPI-taulut kertovat käyttöjärjestelmälle laitteiston rakenteesta, kuten CPU-ytimistä ja keskeytysohjaimista. VNTOS lukee ACPI-tietoja ja käyttää niitä Local APIC -alustuksessa. SMP tarkoittaa usean CPU:n tai ytimen käyttöä samassa järjestelmässä.

Tutki `os/firmware/acpi.vnt`, `os/arch/x86_64/apic.vnt` ja `os/kernel/smp.vnt`. Erota toisistaan CPU:n löytyminen, sen käynnistäminen ja yhteisen muistin turvallinen rinnakkainen käyttö. Nämä ovat eri asioita.

**Pohdi:** Mitä ACPI kertoo kernelille? Miksi toisen CPU:n käynnistys ei vielä takaa SMP-turvallisuutta?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Loppuprojekti: paranna yhtä alijärjestelmää](./25-graduation-project.md) · [Seuraava oppitunti: APIC IPI -viestit →](./28-apic-ipi-service.md)
