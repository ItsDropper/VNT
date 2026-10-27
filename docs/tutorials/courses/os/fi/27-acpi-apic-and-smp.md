# ACPI, APIC ja usea CPU-ydin

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

ACPI-taulut kuvaavat laitteistoa ja voivat kertoa käytettävissä olevista CPU-ytimistä. APIC on keskeytysjärjestelmä, jonka Local APIC toimii kunkin CPU:n yhteydessä. SMP tarkoittaa usean CPU:n tai ytimen käyttöä samassa järjestelmässä.

## Sanasto

MADT on ACPI-taulu, joka kuvaa muun muassa keskeytysarkkitehtuurin tietoja. BSP on käynnistyksen aloittava CPU. AP on muu käynnistettävä CPU. IPI on CPU:lta toiselle lähetettävä keskeytys.

## Käytännön harjoitus

Tutki os/firmware/acpi.vnt, os/arch/x86_64/apic.vnt ja os/kernel/smp.vnt. Seuraa ACPI-taulun löytämistä, CPU-tunnisteiden käsittelyä ja AP:n käynnistystä. Kirjaa, missä arkkitehtuurin matalan tason koodi sijaitsee.

## Yleinen virhe ja turvallisuus

CPU:n käynnistäminen ei tee kernelin kaikista palveluista SMP-turvallisia. Muistivaraaja ja muu jaettu tila voivat edelleen kärsiä kilpailutilanteista.

## Tarkista ymmärtäminen

Mikä ero on BSP:llä ja AP:llä? Miksi käynnistys ei vielä takaa muistiturvallisuutta?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Loppuprojekti: tee todennettava parannus](25-graduation-project.md) · [Seuraava oppitunti: APIC IPI -viestit →](28-apic-ipi-service.md)
