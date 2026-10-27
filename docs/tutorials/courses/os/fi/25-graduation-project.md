# Loppuprojekti: tee todennettava parannus

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Valitse yksi rajattu ongelma, ymmärrä nykyinen toteutus, muuta sitä ja todista testillä, että uusi käytös toimii. Hyvä projekti voi olla selkeämpi virheilmoitus, regressiotesti tai tarkka dokumentaatio todellisesta rajapinnasta.

## Sanasto

Hyväksymiskriteeri kertoo, mistä onnistuminen tunnistetaan. Regressiotesti estää tunnetun virheen palaamisen. Rajoitus kertoo, mitä toteutus ei vielä tue.

## Käytännön harjoitus

Kirjoita ennen koodaamista ongelma, tutkittavat tiedostot, nykyinen käytös, tavoite, testi ja palautussuunnitelma. Toteuta yksi asia kerrallaan ja tarkista diff sekä CI-tulos.

## Yleinen virhe ja turvallisuus

Älä valitse vaikeaa synkronointikorjausta vain siksi, että se kuulostaa hienolta. Se vaatii kääntäjärajapinnan, lukon ja rinnakkaisuustestien ymmärtämistä.

## Tarkista ymmärtäminen

Miten testi epäonnistuu, jos muutos ei toimi? Kirjoita hyväksymiskriteeri yhdellä lauseella.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Hyvä lopputulos ei ole suurin muutos vaan muutos, jonka tarkoitus ja toiminta ovat selvät. Kerro, mikä ongelma ratkaistiin, miten testi osoittaa sen ja mitkä rajoitukset jäivät.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---


## Koodiesimerkki: hyväksymiskriteeri sidotaan itsekoettaan

```vnt
if phys_self_test() == 1 {
    console_puts("Physical allocator self-test: PASS\r\n")
} else {
    console_puts("Physical allocator self-test: FAIL\r\n")
}
```

Tämä on nykyisen komentotulkin käyttämä malli. Ominaisuus ei ole valmis vain siksi, että sen funktio on olemassa: testin pitää tarkistaa myös rajat, virhetilat ja se, ettei testi vahingoita kernelin varaamaa muistia.

**Tee itse:** Määritä lopputyöllesi yksi hyväksymiskriteeri, lisää normaali-, raja- ja virhetilan testi ja näytä tulos QEMU:ssa. Dokumentoi myös, mitä testi ei todista.


## Toteutuslaboratorio: Loppuprojekti

Tässä osiossa rakennat tai validoit konkreettisen osan nykyisestä VNTOS-kernelistä.

**Tutkittavat tiedostot:** `os/kernel.vnt`, `os/mm/phys.vnt`, `os/interrupts.vnt`

**Tavoite:** Valitse ominaisuus ja toteuta se kokonaan testattavaksi. Projektin pitää sisältää toteutus, negatiiviset testit ja rajojen dokumentointi. Älä esitä suunnitelmaa valmiina ominaisuutena.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Määritä yksi mitattava vaatimus ja hyväksymiskriteeri.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

2. **Etsi nykyinen toteutus:** Valitse moduuli ja määritä sen syötteet, paluuarvot ja virhetilat.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

3. **Kirjaa sopimus:** Kirjoita kolme testiä ennen koodimuutosta: normaali, raja ja virhe.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

4. **Tee pienin muutos:** Toteuta ensin pienin onnistuva polku ja lisää rajatarkistukset.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

5. **Tarkista riippuvuudet:** Käännä, käynnistä QEMU:ssa ja tallenna tuloste.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

6. **Käännä:** Kirjaa täsmällisesti mikä toimii, mikä testattiin ja mikä jäi kesken.
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

[← Edellinen oppitunti](./24-build-and-test-the-interrupt-kernel.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./26-physical-memory-and-page-frames.md)
