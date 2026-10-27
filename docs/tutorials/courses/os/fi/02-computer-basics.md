# Tietokoneen osat kernelin näkökulmasta

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

CPU suorittaa käskyjä. RAM säilyttää aktiiviset tiedot nopeasti saatavilla, kun taas SSD tai HDD säilyttää tiedot virran katketessa. Kernel käyttää osoitteita muistiin ja ohjausrajapintoja laitteisiin. Keskeytys ilmoittaa tapahtumasta, joka vaatii CPU:n huomiota.

## Sanasto

Rekisteri on CPU:n pieni tallennuspaikka. Osoite kertoo, mistä muistista luetaan tai minne kirjoitetaan. Keskeytys siirtää suorituksen käsittelijään. Polling tarkoittaa, että CPU tarkistaa laitteen tilan itse.

## Käytännön harjoitus

Piirrä näppäimistön tapahtumaketju: näppäin, laiteohjain, kernel, komentotulkki. Merkitse, missä kohtaa laite voi ilmoittaa keskeytyksellä ja missä kohtaa kernel voi tarkistaa tilaa kyselyllä.

## Yleinen virhe ja turvallisuus

Fyysinen ja virtuaalinen osoite eivät aina ole sama asia. Ne voivat vastata toisiaan alkuvaiheen identiteettikartoituksessa, mutta sivutus voi muuntaa osoitteen.

## Tarkista ymmärtäminen

Mikä ero on RAM-muistilla ja tallennustilalla? Mikä ero on keskeytyksellä ja pollingilla?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

CPU ei ymmärrä tiedoston tai ikkunan käsitettä itsestään. Se suorittaa käskyjä ja lukee tai kirjoittaa muistia. Korkeamman tason ominaisuudet rakentuvat näiden mekanismien päälle.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---


## Koodiesimerkki: tarkista sivutaulun present-bitti

```vnt
fun virt_translate(address: i64) -> i64 {
    if address < 0 || address >= 1073741824 {
        return -1
    }
    let directory: i64 = virt_page_directory_base()
    let directory_index: int = (address / 2097152) % 512
    let pde: i64 = 0
    unsafe {
        pde = mem_read_u64(directory, directory_index * 8)
    }
    if pde % 2 == 0 {
        return -1
    }
    if (pde / 128) % 2 == 1 {
        let base: i64 = pde - (pde % 2097152)
        return base + (address % 2097152)
    }
    // Varsinainen funktio jatkaa tästä 4 KiB:n PTE:n käsittelyyn.
}
```

Tämä on nykyisen `os/mm/virt.vnt`-tiedoston funktio-ote; koodilohko näyttää rajatarkistuksen, PDE:n lukemisen ja 2 MiB:n sivun käsittelyn. Täysi funktio jatkaa 4 KiB:n sivutaulun kautta, jos PDE ei kuvaa suurta sivua. Ote on tarkoitettu lähdekoodin lukemiseen, ei koko funktion korvaajaksi.

**Tee itse:** Jäljitä koko `virt_translate`-funktiota. Selvitä, miten se käsittelee 4 KiB:n PTE:n, puuttuvan kartoituksen ja sivun sisäisen offsetin.

## Toteutuslaboratorio: CPU, portit, muisti ja osoitteet

Tämä osio täydentää käsitteitä käytännön toteutusmenettelyllä. Lähdekoodiviitteet ovat projektin nykyisiä polkuja; avaa ne ennen muutosta.

**Tutkittavat tiedostot:** `os/arch/x86_64/boot.S`, `os/console.vnt`, `os/serial.vnt`

**Tavoite:** Seuraa osoitteen kulkua käskystä laitteeseen. Erota fyysinen osoite, virtuaaliosoite ja I/O-portti. Tee pieni luku- tai kirjoituspolun jäljitys ennen koodimuutosta.

### Tee työ järjestyksessä

1. **Rajaa vastuu:** Etsi VGA-osoite 0xB8000 ja sarjaportin portit.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

2. **Etsi nykyinen toteutus:** Kirjaa kunkin toiminnon käyttämä osoiteavaruus.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

3. **Kirjaa sopimus:** Tarkista, että muistiin kirjoittaminen tapahtuu vain kartoitettuun alueeseen.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

4. **Tee pienin muutos:** Älä korvaa portti-I/O:ta tavallisella muistikirjoituksella.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

5. **Tarkista riippuvuudet:** Tee taulukko rekisteristä, osoitteesta, leveydestä ja tarkoituksesta.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

6. **Käännä:** Varmista QEMU:ssa erikseen näyttö- ja sarjaporttituloste.
   - Kirjaa havainto ennen kuin muokkaat lähdekoodia.
   - Vertaa toteutusta sen kutsujan odotuksiin.

### Selvitä jokainen rajapinta

- Mikä funktio aloittaa toiminnon, ja kuka sitä kutsuu?
- Mitä arvoja funktio lukee, mitä se muuttaa ja mitä se palauttaa?
- Mitkä muistiosoitteet, indeksit tai laitearvot tulevat ulkopuolelta?
- Mikä tarkistus tapahtuu ennen kuin arvoa käytetään?
- Mikä on selkeä virhetilan paluuarvo tai diagnostiikka?
- Mikä osa on VNT:tä ja mikä osa on CPU:n vaatimaa assemblyä?
- Mitä nykyinen toteutus nimenomaisesti ei tue?

### Hyväksymistesti

1. Kirjoita ennen käännöstä odotettu tuloste tai tilamuutos.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Käynnistä syntynyt kernel vain QEMU:ssa.
4. Tarkista sarjaportin tai konsolin tuloste ja vertaa sitä odotukseen.
5. Toista testi rajatapauksella ja virheellisellä syötteellä.
6. Jos et voi suorittaa testiä, ilmoita se avoimesti äläkä merkitse sitä läpäistyksi.

### Yleiset toteutusvirheet

- Uuden funktion lisääminen ilman kutsuja tai importtia: koodi ei ole osa käynnistyspolkua.
- Oletus, että käännös tarkoittaa käynnistymistä.
- Rajojen tarkistaminen vasta sen jälkeen, kun muistia on jo luettu tai kirjoitettu.
- Liian suuri muutos, jonka jälkeen ei tiedetä, mikä rikkoi toiminnan.
- Diagnostiikan lisääminen keskeytyskäsittelijään niin, että alkuperäinen virhe muuttuu uudeksi jumiutumiseksi.

### Muutosraportti

Kirjoita muistiin: (1) tiedostot, joita muutit, (2) uuden toiminnon sopimus, (3) tarkka build-tulos, (4) QEMU-havainto, (5) testatut virhepolut ja (6) rajoitukset, joita et ratkaissut. Tämä raportti on osa toteutusta, ei ylimääräistä paperityötä.

---

[← Edellinen oppitunti](./01-what-is-an-os.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./03-setup.md)
