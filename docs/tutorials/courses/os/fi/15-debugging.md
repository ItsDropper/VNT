# Selvitä ongelmat järjestelmällisesti

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kernelin virhe voi olla käännösvirhe, linkitysvirhe, käynnistysjumi tai CPU-poikkeus. Aloita luokittelemalla ongelma: pääseekö build loppuun, käynnistyykö GRUB ja näkyykö sarjaportissa viestejä? Näin et tutki väärää kerrosta.

## Sanasto

Juurisyy on varsinainen aiheuttaja. Regressio on aiemmin toimineen asian rikkoutuminen. Poikkeus on CPU:n ilmoittama tapahtuma. Minimointi rajaa ongelman pienimpään toistettavaan tapaukseen.

## Käytännön harjoitus

Toista virhe, lue ensimmäinen varsinainen virhe, etsi viimeinen onnistunut vaihe ja tarkastele viimeksi muutettua koodia. Muuta yksi asia kerrallaan ja toista sama testi. Kirjaa tarkka komento ja tuloste.

## Yleinen virhe ja turvallisuus

Älä poista tarkistuksia vain saadaksesi buildin läpi. Varoitus voi kertoa todellisesta virheestä. Jos tulostetta ei näy, konsoli ei ehkä ole vielä alustettu.

## Tarkista ymmärtäminen

Mitä hyvään vikaraporttiin kuuluu? Miksi ensimmäinen virhe on usein viimeistä tärkeämpi?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Hyvä debuggaus muuttaa epävarmuuden testattaviksi vaihtoehdoiksi. Kerro viimeinen toimiva vaihe, tarkka komento ja muutos, jonka jälkeen virhe alkoi. Tämä tekee ongelmasta toistettavan.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---


## Koodiesimerkki: debug-merkki boot-vaiheiden väliin

```asm
movw $0xe9, %dx
movb $66, %al
outb %al, %dx
```

Tätä portti 0xE9 -merkkiä käytetään jo nykyisessä boot.S-tiedostossa. Se on QEMU:n debug-porttiin kirjoitettu tavu, jolla voi merkitä, että suoritus saavutti tietyn kohdan. Käytä eri merkkiä eri vaiheissa, jotta sarjaportin tai debug-lokin viimeisestä merkistä voi rajata pysähdyksen sijainnin.

**Tee itse:** Lisää merkki vain yhteen kohtaan ja testaa. Kun se toimii, lisää toinen epäillyn vaiheen jälkeen. Älä muuta useita boot-vaiheita samalla kertaa.


## Toteutuslaboratorio: Debuggaus

Tässä osiossa teet konkreettisen muutoksen nykyiseen VNTOS-koodiin. Avaa lähdetiedostot ja varmista, että kuvaus vastaa nykyistä toteutusta.

**Tutkittavat tiedostot:** `os/build.sh`, `os/arch/x86_64/boot.S`, `os/interrupts.vnt`

**Tavoite:** Paikanna vika viimeisen onnistuneen vaiheen avulla. Jokainen debug-merkki vastaa yhteen kysymykseen. Älä korjaa oiretta ennen kuin tiedät ensimmäisen virheen.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Jaa käynnistys vaiheisiin ja merkitse jokaisen vaiheen viimeinen onnistunut tuloste.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

2. **Etsi nykyinen toteutus:** Jos build epäonnistuu, selvitä ensimmäinen virhe ja sen lähde.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

3. **Kirjaa sopimus:** Jos boot pysähtyy, lisää kaksi merkkiä epäillyn vaiheen ympärille.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

4. **Tee pienin muutos:** Jos keskeytys kaatuu, tarkista pino, vektori, virhekoodi ja paluukäsky.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

5. **Tarkista riippuvuudet:** Jos muisti vioittuu, lisää rajatarkistukset ennen dereferointia.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

6. **Käännä:** Poista väliaikainen tuloste vasta kun testaat korjauksen ilman sitä.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

7. **Tarkista konekoodi:** Kirjaa tarkka toistoresepti, ei vain “ei toimi”.
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

[← Edellinen oppitunti](./14-the-serial-shell.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./16-testing.md)
