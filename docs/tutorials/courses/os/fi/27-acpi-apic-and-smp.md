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


## Syvempi harjoitus: seuraa koodia vaiheittain

Erota löydetty CPU, käynnistetty CPU ja CPU, joka osallistuu turvallisesti jaetun tilan käsittelyyn. Nämä ovat eri tasoja ja jokainen vaatii oman todisteensa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---


## Koodiesimerkki: tarkista ACPI-taulun checksum

```vnt
fun acpi_checksum(address: i64, length: int) -> int {
    if address <= 0 || length <= 0 || address + length > 1073741824 {
        return 0
    }
    let sum: int = 0
    let i: int = 0
    while i < length {
        sum = (sum + acpi_read8(address, i)) % 256
        i = i + 1
    }
    if sum == 0 {
        return 1
    }
    return 0
}
```

Tämä on nykyisen os/firmware/acpi.vnt-tiedoston funktio. ACPI-taulun checksum tarkistetaan laskemalla tavut yhteen modulo 256; kelvollisen taulun summa on nolla. Ennen lukemista tarkistetaan osoite, pituus ja se, että alue pysyy bootstrapin kartoittamassa muistissa.

**Tee itse:** Tarkista, että jokainen ACPI-tietorakenne validoidaan ennen sen kenttien käyttöä. MADT:n muuttuvamittaisia merkintöjä kuljetaan entry_length-kentän mukaan ja pituus tarkistetaan ennen kenttien lukemista.


## Toteutuslaboratorio: ACPI, APIC ja SMP

Tässä osiossa rakennat tai validoit konkreettisen osan nykyisestä VNTOS-kernelistä.

**Tutkittavat tiedostot:** `os/firmware/acpi.vnt`, `os/arch/x86_64/apic.vnt`, `os/kernel/smp.vnt`

**Tavoite:** Etene firmware-löydöstä AP:n hallittuun käynnistykseen. ACPI-tiedot, MMIO ja APIC ID:t pitää validoida ennen käyttöä. CPU:n löytyminen ei todista sen käynnistyneen.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Tarkista RSDP-allekirjoitus, checksum ja taulun pituus.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

2. **Etsi nykyinen toteutus:** Kulje MADT:n muuttuvamittaiset merkinnät vain taulun rajojen sisällä.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

3. **Kirjaa sopimus:** Varmista IOAPIC-osoite ennen MMIO-lukua ja pidä reitit maskattuina.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

4. **Tee pienin muutos:** Tarkista APIC ID:t, trampoliinin koko ja entry/stack-slotit ennen AP:n käynnistystä.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

5. **Tarkista riippuvuudet:** Testaa ensin firmware-tietojen löytyminen, sitten APICin alustus, sitten yksi AP.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

### Turvallisuus- ja rajapintasopimus

- Validoi osoite, pituus, indeksi, tunniste ja laitetila ennen käyttöä.
- Älä lue muuttuvamittaista tietorakennetta ilman rajojen tarkistusta.
- Määritä selkeä virhepolku puuttuvalle laitteelle, virheelliselle datalle ja aikakatkaisulle.
- Erota “havaittu”, “alustettu”, “online” ja “käytössä” eri tiloiksi, jos ne tarkoittavat eri asioita.
- Älä oleta, että atominen muistikirjoitus yksin ratkaisee muistijärjestyksen tai omistajuuden.
- Dokumentoi rajoitukset, joita nykyinen kernel ei vielä ratkaise.

### Todennus

1. Kirjoita odotettu tila ja havainto ennen muutosta.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut komento ja erottele build-ongelma laiteongelmasta.
4. Käynnistä vain QEMU:ssa ja kerää sarjaportin tuloste.
5. Testaa normaali tapaus, rajatapaus ja virheellinen tai puuttuva tieto.
6. Toista testi, jos toiminta riippuu ajoituksesta.
7. Älä väitä CI:n tai laitetestin läpäisseen, ellei siitä ole todellista tulosta.

### Tavalliset virheet

- Firmwaresta saatuun tietoon luotetaan ennen checksum- ja pituustarkistuksia.
- AP merkitään online-tilaan ennen kuin se saavuttaa sovitun valmiuspisteen.
- IPI lähetetään ennen kuin IDT ja vastaanottaja ovat valmiita.
- Jaettua muistia käytetään ilman omistajuus- tai lukitussopimusta.
- Laskurin kasvaminen tulkitaan liian laajasti todisteeksi ominaisuuden valmiudesta.

### Muutosraportti

Kirjaa muutetut tiedostot, rajapinta, hyväksymiskriteeri, build-tulos, QEMU-havainto, negatiiviset testit ja jäljelle jääneet rajoitukset.

---

[← Edellinen oppitunti](./26-physical-memory-and-page-frames.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./28-apic-ipi-service.md)
