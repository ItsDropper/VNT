# Miten jatkaa kurssin jälkeen

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Valitse yksi alijärjestelmä ja seuraa sitä toteutuksesta testeihin. Hyviä aiheita ovat keskeytykset, fyysisen muistin varaaja, ACPI ja APIC. Älä aloita suurta uudelleenkirjoitusta ennen kuin pystyt selittämään nykyisen toiminnan ja sen rajoitukset.

## Sanasto

Alijärjestelmä on rajattu osa. Rajapinta määrittää, miten sitä kutsutaan. Invariantti on sääntö, jonka pitää pysyä totena, kuten ettei samaa vapaata sivukehystä anneta kahdelle käyttäjälle.

## Käytännön harjoitus

Valitse aihe, etsi toteutustiedosto, kaikki kutsujat ja testit. Kirjoita kartta: mitä dataa osa omistaa, mitä funktioita se tarjoaa ja mikä voi mennä vikaan. Tee vasta sitten rajattu muutos.

## Yleinen virhe ja turvallisuus

Ominaisuuden lisääminen vaatii usein virhetilanteiden käsittelyä, testejä ja rajapinnan suunnittelua. Moniydinkoodissa pitää lisäksi miettiä samanaikainen käyttö.

## Tarkista ymmärtäminen

Valitse jatkoaihe ja nimeä kolme asiaa, jotka tutkit ennen koodaamista.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Aloita uuden alijärjestelmän julkisesta rajapinnasta ja seuraa kutsuja sisäänpäin. Kirjaa, kuka omistaa datan ja mitkä ehdot sen pitää täyttää. Tämä auttaa tekemään rajattuja muutoksia.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Tee ensimmäinen pieni muutos](17-first-kernel-change.md) · [Seuraava oppitunti: Jaa kernel VNT-moduuleihin →](19-split-the-kernel-into-vnt-modules.md)

## Koodiesimerkki: käytä olemassa olevaa itsekoetta ennen uutta ominaisuutta

```vnt
if phys_self_test() == 1 {
    console_puts("Physical allocator self-test: PASS\r\n")
} else {
    console_puts("Physical allocator self-test: FAIL\r\n")
}
```

Tämä tarkistus on käytössä os/kernel/shell.vnt-tiedoston memtest-komennossa. Se kutsuu allokaattorin omaa testiä ja käsittelee onnistumisen sekä epäonnistumisen erikseen. Ennen kuin rakennat riippuvaista ominaisuutta, varmista ensin, että sen alijärjestelmän nykyinen testi antaa odotetun tuloksen.

**Tee itse:** Käynnistä VNTOS QEMU:ssa ja suorita `memtest` sekä `vmtest`. Kirjaa tulokset. Valitse seuraavaksi vain sellainen ominaisuus, jonka riippuvuudet ovat jo olemassa.


## Toteutuslaboratorio: Kurssin jälkeinen työ

Tässä osiossa teet konkreettisen muutoksen nykyiseen VNTOS-koodiin. Avaa lähdetiedostot ja varmista, että kuvaus vastaa nykyistä toteutusta.

**Tutkittavat tiedostot:** `os/README.md`, `os/kernel.vnt`, `os/mm/phys.vnt`

**Tavoite:** Valitse seuraava ominaisuus riippuvuuksien, ei näyttävyyden perusteella. Tee pohjatyö ennen kuin rakennat schedulerin tai käyttäjätilat. Rajaa tavoite yhteen mitattavaan tulokseen.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Tarkista mikä nykyisistä alijärjestelmistä on valmis ja mikä vasta kokeellinen.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

2. **Etsi nykyinen toteutus:** Piirrä riippuvuusketju ennen uuden moduulin luontia.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

3. **Kirjaa sopimus:** Valitse yksi pieni ominaisuus ja määritä sen virhetilat.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

4. **Tee pienin muutos:** Kirjoita testisuunnitelma ennen toteutusta.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

5. **Tarkista riippuvuudet:** Tee vaiheittainen toteutus ja dokumentoi keskeneräiset osat.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

6. **Käännä:** Älä nimeä laskuria tai AP:n käynnistystä valmiiksi ajastukseksi.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

7. **Tarkista konekoodi:** Valitse seuraava työ vasta kun nykyinen muutos on todennettu.
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
