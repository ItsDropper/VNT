# SMP-turvallisuus: synkronoi ennen jaettua muistia

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kun usea CPU muuttaa samaa tietoa, operaatiot voivat limittyä. Race condition eli kilpailutilanne syntyy, kun lopputulos riippuu ajoituksesta. Esimerkiksi kaksi CPU:ta voi nähdä saman sivukehyksen vapaana ja varata sen molemmat.

## Sanasto

Kriittinen osio käsittelee jaettua tilaa, eikä vastaavaa osiota saa suorittaa samanaikaisesti. Lukko antaa yhdelle CPU:lle kerrallaan pääsyn. Atominen operaatio näkyy määritellyllä tavalla yhtenä tapahtumana myös muille CPU:ille.

## Käytännön harjoitus

Tutki os/mm/phys.vnt ja merkitse kohdat, joissa bittikartta ja laskuri muuttuvat. Kuvittele kahden CPU:n suorittavan saman varauksen yhtä aikaa. Kirjoita aikajana siitä, miten molemmat voivat lukea saman vapaan bitin ennen muutosta.

## Yleinen virhe ja turvallisuus

volatile ei ole lukko eikä tee usean käskyn kokonaisuudesta atomista. Inline-assembly ilman kääntäjälle ilmoitettuja muistivaikutuksia ei takaa oikeaa järjestystä. VNTOSin IPI-testi ei tee varaajasta SMP-turvallista.

## Tarkista ymmärtäminen

Piirrä kahden CPU:n kilpailutilanne. Missä kohdassa lukko tai atominen operaatio estäisi virheen?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Synkronointi on sekä laitteiston että kääntäjän sopimus. Lukon pitää estää samanaikainen kriittinen osio ja kääntäjän säilyttää muistitoimintojen järjestys. volatile ei korvaa kumpaakaan.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---


## Koodiesimerkki: nykyinen AP-tila ei ole vielä yleinen lukitus

```vnt
if acpi_cpu_apic_id(index) == apic_id {
    state_set(112 + index, 1)
    state_set(60, state_get(60) + 1)
    matched = 1
}
```

Tämä on nykyisen os/kernel/smp.vnt-tiedoston AP:n alustuspolusta. Se julkaisee AP:n online-tilan ja kasvattaa jaettua laskuria. Tämä on rajattu nykyiseen malliin, jossa AP ei aja scheduleria eikä käsittele tavallisia laite-IRQ:ita. `state_get` + `state_set` ei ole yleinen atominen read-modify-write-operaatio, joten sitä ei voi turvallisesti käyttää rinnakkaiseen allokointiin tai monen CPU:n lukitukseen.

**Tee itse:** Ennen kuin sallit usean CPU:n muokata samaa tietoa, määritä omistaja, atomisuus, muistijärjestys ja lukituksen deadlock-säännöt. Nykyinen SMP-toteutus ei vielä tarjoa yleistä scheduler- tai allokaattoriturvallista synkronointia.


## Toteutuslaboratorio: SMP-synkronointi

Tässä osiossa rakennat tai validoit konkreettisen osan nykyisestä VNTOS-kernelistä.

**Tutkittavat tiedostot:** `os/kernel/smp.vnt`, `os/mm/phys.vnt`, `os/kernel/core.vnt`

**Tavoite:** Määritä jaetun tilan omistajuus ennen rinnakkaista käyttöä. Yhden CPU:n keskeytysten estäminen ei lukitse muita CPU:ita. Älä käynnistä rinnakkaista allokointia ennen lukituksen sopimusta.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Luettele jaettu tila ja merkitse jokaisen muuttujan omistaja.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

2. **Etsi nykyinen toteutus:** Kirjaa, voiko tila muuttua BSP:ltä, AP:ltä, IRQ:sta vai kaikista.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

3. **Kirjaa sopimus:** Valitse atominen operaatio, spinlock, per-CPU-tila tai yksittäinen omistaja käyttötapauksen perusteella.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

4. **Tee pienin muutos:** Määritä mitä tapahtuu, jos keskeytys yrittää ottaa saman CPU:n jo pitämän lukon.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

5. **Tarkista riippuvuudet:** Tee toistuva rinnakkaistesti ja vertaa laskuria odotettuun arvoon.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

6. **Käännä:** Pidä AP turvallisessa idle-tilassa, jos jaetun muistin sopimusta ei vielä ole.
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

[← Edellinen oppitunti](./28-apic-ipi-service.md) · [Sisällysluettelo](./README.md) · **Kurssi suoritettu.**
