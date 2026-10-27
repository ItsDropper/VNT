# APIC IPI -viestit

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

IPI eli prosessorien välinen keskeytys on viesti, jonka yksi CPU lähettää toiselle. VNTOSin nykyinen testi lähettää kiinteän vektorin IPI-viestejä ja tarkistaa, että kohde-CPU käsittelee ne.

Tutki `os/kernel/smp.vnt` ja `os/arch/x86_64/apic.vnt`. Etsi lähetys, keskeytyskäsittelijä ja vastauslaskuri. Testi osoittaa, että CPU voi vastaanottaa IPI:n ja vastata siihen. Se ei todista, että muistivaraaja tai muut jaetun tilan palvelut olisivat turvallisia samanaikaisessa käytössä.

**Pohdi:** Mitä nykyinen IPI-testi todistaa ja mitä se ei todista?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: ACPI, APIC ja usean CPU:n käynnistys](./27-acpi-apic-and-smp.md) · [Seuraava oppitunti: SMP-turvallisuus: synkronointi ennen yhteistä muistia →](./29-smp-synchronization-contract.md)
