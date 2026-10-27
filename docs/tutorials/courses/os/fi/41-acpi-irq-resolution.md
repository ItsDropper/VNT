# ACPI:n ISA-keskeytysten reititysten selvittäminen

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Opit, miten vanhat ISA IRQ -numerot muunnetaan ACPI-järjestelmän Global System Interrupt (GSI) -numeroiksi. Toteutamme vain lukevan rajapinnan: se ei ota laitteistoreitityksiä käyttöön.

## Miksi IRQ ja GSI eivät aina ole sama numero?

Vanhojen laitteiden koodi käyttää usein IRQ0–IRQ15-numeroita. I/O APIC reitittää sen sijaan GSI-keskeytyksiä. ACPI:n Multiple APIC Description Table (MADT) voi sisältää Interrupt Source Override (ISO) -merkintöjä, jotka muuttavat ISA-IRQ:n GSI-kohteen ja määrittävät sähköisen napaisuuden sekä liipaisutilan.

Siksi oletus `GSI = IRQ` ei aina pidä paikkaansa. Lähde-IRQ ja kohde-GSI ovat eri käsitteitä, ja niiden sekoittaminen voi johtaa väärän keskeytysreitin ohjelmointiin.

## Toteutus

Tiedoston `os/firmware/acpi.vnt` MADT-jäsennin tallentaa enintään 16 ohitusmerkintää. Jokaisesta tallennetaan lähde-IRQ, kohde-GSI ja raakamuotoinen 16-bittinen lippuarvo.

Uusi `acpi_irq_gsi(irq)` tarkistaa, että IRQ on välillä 0–15, ja etsii sitä vastaavan ohitusmerkinnän. Jos merkintää ei ole, funktio palauttaa tavanomaisen identiteettikuvauksen eli `GSI = IRQ`. Paluuarvo `-1` tarkoittaa virheellistä syötettä ja `-2` päällekkäisiä, keskenään epäselviä lähde-IRQ-merkintöjä.

`acpi_irq_override_flags(irq)` käyttää samaa hakuperiaatetta. Se palauttaa nollan, jos ohitusta ei ole, raakaliput yhden osuman tapauksessa, `-1` virheellisestä syötteestä ja `-2` päällekkäisistä merkinnöistä. Lippuja ei tässä vaiheessa tulkita napaisuudeksi tai liipaisutilaksi.

Shellin `irq`-komento näyttää nyt IRQ0:n ja IRQ1:n tehokkaat GSI-numerot sekä raakamuotoiset ohitusliput. `acpitest`-komento vertaa kaikki 16 IRQ-arvoa jäsennettyyn ohitustaulukkoon ja testaa virheelliset IRQ-syötteet. Se ilmoittaa SKIP, jos MADT-taulukkoa ei löytynyt, ja FAIL, jos lähde-IRQ on päällekkäinen tai ratkaistu arvo ei vastaa taulukkoa. QEMU-työnkulku suorittaa komennon ja tarkistaa PASS-tuloksen sarjaportin tulosteesta. CI tarkistaa myös diagnostiikkakenttien olemassaolon olettamatta tietyn emulaattorin tai laiteohjelmiston tarkkaa ohitustaulukkoa.

## Turvaraja

Funktiot vain lukevat ja ratkaisevat laiteohjelmiston tietoja. Ne eivät kirjoita I/O APIC -rekistereihin, avaa keskeytysreittejä tai ota laitekeskeytyksiä käyttöön. Ennen tulevaa reititystoteutusta pitää tarkistaa I/O APICin GSI-alue, tulkita napaisuus- ja liipaisuliput, valita keskeytysvektori, asentaa käsittelijä ja määrittää keskeytyksen omistajuus.

Nykyinen ydin pitää I/O APIC -reitit peitettyinä. Tätä turvarajaa ei pidä poistaa vain siksi, että GSI-haku palauttaa uskottavan numeron.

## Harjoitukset

1. Selitä lähde-IRQ:n ja kohde-GSI:n ero.
2. Miksi kahdesta samalle lähde-IRQ:lle osuvasta merkinnästä pitää ilmoittaa epäselvyytenä ensimmäisen valitsemisen sijasta?
3. Miksi oletusreitin liput ovat nolla?
4. Luettele tarkistukset, joita tarvitaan ennen I/O APIC -reitin avaamista.

## Hyväksymistarkistus

- [ ] IRQ-arvot 0–15:n ulkopuolella hylätään.
- [ ] Yksittäinen ISO-merkintä ohittaa identiteettikuvauksen.
- [ ] Puuttuva ohitus käyttää identiteettikuvausta ja nollalippuja.
- [ ] Päällekkäiset lähde-IRQ-merkinnät ilmoitetaan epäselviksi.
- [ ] Shell näyttää IRQ0:n ja IRQ1:n GSI-numerot ja raakamuotoiset liput.
- [ ] `acpitest` vertaa kaikki 16 IRQ-arvoa jäsennettyyn taulukkoon.
- [ ] CI tarkistaa tulosteen kentät olettamatta tiettyä laiteohjelmiston taulukkoa.
- [ ] Muutos ei avaa I/O APIC -reittejä.

---

[← Edellinen oppitunti](./40-smp-atomic-counter-self-test.md) · [Kurssin hakemisto](./README.md)
