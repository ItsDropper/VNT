# APIC IPI -viestit

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

IPI on keskeytys, jonka yksi CPU lähettää toiselle. VNTOSin testipolku lähettää kiinteän vektorin IPI-viestejä ja tarkistaa, että kohde-CPU käsittelee ne. Näin voidaan testata CPU-ytimien välistä ilmoituspolkua.

## Sanasto

Lähettäjä lähettää viestin, kohde vastaanottaa sen ja vektori valitsee käsittelijän. Vastauslaskuri voi antaa testille näkyvän merkin käsittelijän suorituksesta.

## Käytännön harjoitus

Avaa os/kernel/smp.vnt ja os/arch/x86_64/apic.vnt. Etsi IPI:n lähetys, kohteen keskeytyskäsittelijä ja vastauslaskuri. Vertaa testin odotettua ja havaittua tulosta.

## Yleinen virhe ja turvallisuus

IPI-testi osoittaa viestin toimituksen kyseisessä testissä. Se ei todista, että muistivaraaja tai kaikki jaettu tila olisi SMP-turvallista, eikä se ole tehtäväajastin.

## Tarkista ymmärtäminen

Mitä IPI-testi todistaa? Nimeä kaksi asiaa, joita se ei todista.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

IPI:n testaamisessa lähetys ei riitä: kohteen käsittely pitää todentaa. Silti testin tulos koskee vain polkua, jonka testi suorittaa, eikä sitä pidä yleistää muihin alijärjestelmiin.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: ACPI, APIC ja usea CPU-ydin](27-acpi-apic-and-smp.md) · [Seuraava oppitunti: SMP-turvallisuus: synkronoi ennen jaettua muistia →](29-smp-synchronization-contract.md)

## Toteutuslaboratorio: APIC IPI -viestit

Tässä osiossa rakennat tai validoit konkreettisen osan nykyisestä VNTOS-kernelistä.

**Tutkittavat tiedostot:** `os/arch/x86_64/apic.vnt`, `os/kernel/smp.vnt`, `os/interrupts.vnt`

**Tavoite:** Toteuta IPI-polku, jonka vastaanotto voidaan laskea ja todentaa. Vektori, kohde-CPU, toimitustila ja EOI on tarkistettava. IPI ei itsessään ole tehtäväajastin tai yleinen viestijono.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Varmista että vektori 240 on asennettu IDT:hen ja ohjautuu VNT-dispatcheriin.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

2. **Etsi nykyinen toteutus:** Varmista, että vastaanottaja on online ja kohde-ID on oikea.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

3. **Kirjaa sopimus:** Määritä viestin semantiikka ennen lähettämistä.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

4. **Tee pienin muutos:** Lisää vastaanotosta rajattu laskuri tai tila, älä tulosta keskeytyksessä.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

5. **Tarkista riippuvuudet:** Testaa lähetettyjen ja vastaanotettujen viestien määrä yhdellä AP:lla.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

6. **Käännä:** Jos viesti ei saavu, tarkista APIC enable, IF, vektori ja toimitustila.
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
