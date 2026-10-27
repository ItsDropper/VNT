# Tee ensimmäinen pieni muutos

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Ensimmäisen muutoksen pitää olla pieni ja helposti tarkistettava. Selkeämpi shellin virheilmoitus on parempi harjoitus kuin keskeytysten tai muistinhallinnan uudelleenkirjoitus. Ennen muokkausta selvitä nykyinen toiminta ja miten uuden käytöksen voi todistaa.

## Sanasto

Diff näyttää muutokset. Rajattu muutos muuttaa yhtä asiaa. Palautettava tila tarkoittaa, että voit palata tunnettuun versioon, jos kokeilu epäonnistuu.

## Käytännön harjoitus

Lue funktio, kirjoita nykyinen toiminta ylös, tee yksi muutos, tarkista diff, rakenna kernel, käynnistä QEMU ja suorita siihen liittyvä testi. Jos tulos muuttuu odottamattomasti, selvitä syy ennen seuraavaa muutosta.

## Yleinen virhe ja turvallisuus

Älä tee monta riippumatonta muutosta kerralla. Muuten et tiedä, mikä niistä aiheutti virheen. Älä myöskään väitä testanneesi toimintaa, jos tarkistit vain lähdekoodin.

## Tarkista ymmärtäminen

Suunnittele pieni muutos: tiedosto, uusi käytös, testi ja palautussuunnitelma.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Ennen muokkausta tallenna lähtötilanne ja määritä hyväksymiskriteeri. Muutoksen jälkeen vertaa toteutunutta käytöstä siihen, mitä kirjoitit etukäteen. Pieni muutos on hyödyllinen vain, jos sen vaikutus voidaan todentaa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---


## Koodiesimerkki: tee ensimmäinen näkyvä VNT-muutos

```vnt
fun kernel_main() {
    serial_init()
    serial_puts("MY_FIRST_KERNEL_CHANGE\r\n")
    vga_clear()
    vga_puts("VNTOS\r\n")
    // Nykyinen alustus jatkuu tässä.
    return 0
}
```

Tämä on pieni havainnollistava esimerkki nykyisen kernel_main-rakenteen pohjalta. Jos lisäät uuden viestin olemassa olevaan funktioon, säilytä sen nykyinen sisältö ja alustusjärjestys; älä korvaa koko kernel_main-funktiota tällä lyhennetyllä esimerkillä.

**Tee itse:** Lisää yksi serial_puts-kutsu olemassa olevaan os/kernel.vnt-tiedostoon ennen shell_run-kutsua. Käännä, käynnistä QEMU:ssa ja varmista että juuri uusi tunniste näkyy sarjaportissa.


## Toteutuslaboratorio: Ensimmäinen kernel-muutos

Tässä osiossa teet konkreettisen muutoksen nykyiseen VNTOS-koodiin. Avaa lähdetiedostot ja varmista, että kuvaus vastaa nykyistä toteutusta.

**Tutkittavat tiedostot:** `os/kernel.vnt`, `os/serial.vnt`, `os/console.vnt`

**Tavoite:** Toteuta pieni muutos, jonka vaikutus näkyy ulospäin. Pidä muutoksen koko pienenä ja tarkista se erikseen. Älä muuta samalla kerralla laitteistoa, buildia ja muistia.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Valitse yksilöivä käynnistysviesti ja päätä, missä vaiheessa sen pitäisi näkyä.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

2. **Etsi nykyinen toteutus:** Muuta yhtä VNT-tiedostoa ja säilytä nykyinen funktiosopimus.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

3. **Kirjaa sopimus:** Käännä ja varmista, että muutos sisältyy kernelin assemblyyn.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

4. **Tee pienin muutos:** Käynnistä QEMU:ssa ja tarkista sarjaportti.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

5. **Tarkista riippuvuudet:** Poista tai muuta viesti erillisessä testissä varmistaaksesi, että katsot oikeaa buildia.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

6. **Käännä:** Testaa palautus alkuperäiseen tilaan.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

7. **Tarkista konekoodi:** Kirjaa, mitä havainto todistaa ja mitä se ei todista.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

### Toteutuksen sopimus

- Nimeä toiminnon omistava moduuli ja julkinen funktio.
- Kirjaa parametrien ja paluuarvon merkitys sekä epäonnistumisen tapa.
- Tarkista kaikki rajat ennen muistilukua, muistikirjoitusta tai portti-I/O:ta.
- Tarkista, että jokainen import, funktiokutsu ja linkitetty symboli todella on olemassa.
- Pidä CPU:n vaatima assembly erillään VNT:n päätöksenteosta.
- Dokumentoi ominaisuudet, joita tämä muutos ei vielä toteuta.

### Todennus

1. Kirjoita ennuste ennen muutosta.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut vaihe ja sen tarkka syy.
4. Käynnistä kernel QEMU:ssa ja kerää sarjaportin tuloste.
5. Testaa normaali polku, rajatapaus ja virhepolku.
6. Vertaa tulosta ennusteeseen; älä laske pelkkää käännöstä toiminnalliseksi testiksi.

### Tavalliset virheet

- Usean alijärjestelmän muuttaminen samassa erässä.
- Testin tekeminen eri buildista kuin jota juuri muokkasit.
- Importin tai linkkerin riippuvuuden unohtaminen.
- Väitteen tekeminen keskeneräisestä ominaisuudesta.
- Virhepolun ohittaminen, koska normaali tapaus toimii.

### Muutosraportti

Kirjaa muutetut tiedostot, sopimus, build-komento ja tulos, QEMU-havainto, negatiiviset testit sekä avoimet rajoitukset. Jos jokin tarkistus jäi tekemättä, ilmoita se selvästi.

---

[← Edellinen oppitunti](./16-testing.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./18-next-steps.md)
