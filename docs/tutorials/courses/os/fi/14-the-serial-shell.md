# Sarjaportin komentotulkki

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Shell lukee tekstin, tunnistaa komennon ja kutsuu oikeaa toimintoa. Tyypillinen polku on syötteen lukeminen, rivin käsittely, komennon vertailu, käsittelijän kutsu ja tulostus.

## Sanasto

Jäsentäminen muuttaa tekstin ohjelman käsittelemäksi rakenteeksi. Komentokäsittelijä toteuttaa yhden komennon. Virheilmoitus kertoo, miksi syötettä ei voitu käsitellä.

## Käytännön harjoitus

Avaa os/kernel/shell.vnt. Etsi syötepuskuri, komennon tunnistus ja tuntemattoman komennon käsittely. Seuraa yhden komennon polkua alusta tulosteeseen ennen kuin lisäät uuden komennon.

## Yleinen virhe ja turvallisuus

Tarkista tyhjä syöte, liian pitkä rivi ja tuntematon komento. Älä anna käyttäjän tekstin toimia muistiosoitteena tai rajattoman puskurin sisältönä.

## Tarkista ymmärtäminen

Piirrä polku näppäimistöstä komentokäsittelijään. Missä kohtaa virheellinen syöte pitäisi hylätä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Seuraa samaa syötemuuttujaa koko komentopolun läpi. Näin huomaat, missä välilyönnit, rivin loppu ja tuntematon nimi käsitellään. Tarkista myös, ettei uusi komento riko vanhoja komentoja.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Sarjaportti debuggausvälineenä](13-serial-io.md) · [Seuraava oppitunti: Selvitä ongelmat järjestelmällisesti →](15-debugging.md)

## Koodiesimerkki: lisää komentotulkkiin komento

```vnt
if str_eq(buffer, "about") {
    console_puts("VNTOS: VNT-compiled freestanding x86-64 kernel.\r\n")
    handled = 1
}
if handled == 0 {
    console_puts("Unknown command. Type help.\r\n")
}
```

Tämä on nykyisen os/kernel/shell.vnt-tiedoston rakennetta vastaava katkelma: syöte kerätään puskuriin, rivinvaihto päättää komennon ja str_eq vertailee sitä. `handled` kertoo komentotulkille, tunnistiko jokin haara komennon. Oikeassa tiedostossa tuntemattoman komennon haara sijaitsee käsittelyketjun lopussa.

**Tee itse:** Lisää komento samalla mallilla, päivitä `help`-tuloste ja testaa tyhjä rivi, tuntematon komento ja pitkä syöte. Älä lisää komentoa, joka lukee tai kirjoittaa mielivaltaista muistia.


## Toteutuslaboratorio: Sarjaportin komentotulkki

Tässä harjoituksessa toteutat tai jäljität toimivan osan, et vain kuvaile käsitettä.

**Tutkittavat tiedostot:** `os/kernel/shell.vnt`, `os/serial.vnt`, `os/kernel.vnt`

**Tavoite:** Lisää komentotulkin toiminto ilman puskurin ylivuotoa. Shellin pitää hylätä virheellinen syöte hallitusti. Älä anna komentojen kirjoittaa mielivaltaisiin muistiosoitteisiin.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Etsi syötteen keräys, rivinvaihdon käsittely ja komentojen tunnistus.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

2. **Etsi nykyinen toteutus:** Määritä puskurin maksimipituus ja mitä tapahtuu sen täyttyessä.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

3. **Kirjaa sopimus:** Lisää uusi komento tunnistuskohtaan ja erota toiminto tulosteesta.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

4. **Tee pienin muutos:** Tarkista argumenttien määrä ja arvot ennen alijärjestelmäkutsua.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

5. **Tarkista riippuvuudet:** Testaa tyhjä rivi, tuntematon komento, pitkä rivi ja puuttuva argumentti.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

6. **Käännä:** Varmista, että virheellinen komento palaa komentokehotteeseen.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

### Sopimus ja rajat

- Määritä syötteet, paluuarvot ja virhetilanteet ennen toteutusta.
- Tarkista osoitteet, pituudet, indeksit ja tilan alustaminen ennen käyttöä.
- Erota CPU:n vaatima mekanismi VNT:n päätöksenteosta.
- Älä väitä nykyisen rajoitetun toteutuksen tukevan ominaisuuksia, joita koodi ei sisällä.
- Tarkista, ettei uusi toiminto vaadi alustamatonta moduulia tai linkittämätöntä symbolia.
- Pidä diagnostiikka rajattuna ja vältä odottamista keskeytyskontekstissa.

### Testaa jokainen polku

1. Kirjoita ennuste ennen käännöstä.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut komento.
4. Käynnistä kernel QEMU:ssa ja tallenna sarjaportin tuloste.
5. Testaa normaali tapaus, rajatapaus ja virheellinen syöte.
6. Tarkista ettei virheellinen pyyntö muuta muistia tai laitteiston tilaa.
7. Jos testaus ei ole käytettävissä, merkitse tulos vahvistamattomaksi.

### Tavalliset virheet

- Osoitteen tai pituuden tarkistus tehdään vasta luku-/kirjoitusoperaation jälkeen.
- Uusi funktio ei ole mukana import-ketjussa.
- Paluuarvon merkitys ei ole sama kutsujalle ja toteuttajalle.
- Diagnostiikka peittää alkuperäisen vian.
- Käännöksen onnistumista pidetään toimintatestinä.

### Kirjaa toteutus

Dokumentoi muutetut tiedostot, funktiosopimus, build-tulos, QEMU-havainto, negatiiviset testit ja rajoitukset. Näin toinen kehittäjä pystyy toistamaan tarkistuksen.
