# Jaa kernel VNT-moduuleihin

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Moduulit jakavat vastuut: käynnistys, keskeytykset, shell ja muistinhallinta ovat eri tehtäviä. Hyvä moduuli tarjoaa pienen rajapinnan ja pitää sisäiset yksityiskohdat erillään. Tavoite ei ole maksimoida tiedostojen määrää vaan tehdä vastuut ymmärrettäviksi.

## Sanasto

Moduuli on yhteen tehtävään keskittyvä koodiosa. Riippuvuus tarkoittaa, että osa tarvitsee toisen palvelua. Omistajuus kertoo, mikä osa saa muuttaa tiettyä dataa.

## Käytännön harjoitus

Tutki os/kernel.vnt, os/interrupts.vnt, os/mm/phys.vnt ja os/kernel/shell.vnt. Piirrä nuolet funktiokutsujen mukaan. Selvitä build-skriptistä, miten VNT-tiedostot otetaan mukaan käännökseen.

## Yleinen virhe ja turvallisuus

Tiedoston jakaminen ei yksin luo hyvää moduulirajaa. Jos kaikki osat muuttavat samaa globaalia tilaa ilman sääntöjä, kokonaisuus pysyy vaikeana ja virhealttiina.

## Tarkista ymmärtäminen

Piirrä kolmen moduulin riippuvuuskaavio. Mitä moduulijako ratkaisee ja mitä se ei ratkaise?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Hyvä moduuliraja vastaa kysymykseen: kuka on vastuussa tästä tiedosta ja toiminnosta? Jos moduulit tuntevat toistensa sisäiset yksityiskohdat, rajapinta voi olla epäselvä. Tiedostojen määrä ei itsessään mittaa laatua.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---


## Koodiesimerkki: tuo moduuli ja kutsu sen rajapintaa

```vnt
import "serial.vnt"
import "console.vnt"
import "kernel/core.vnt"

fun kernel_main() {
    serial_init()
    console_puts("Kernel modules are reachable.\r\n")
    return 0
}
```

Tämä näyttää nykyisen kernelin import-tyylin ja olemassa olevien moduulien funktiokutsut. `import` tuo lähdetiedoston käännösyksikköön; se ei automaattisesti luo erillistä prosessia tai itsenäisesti linkitettävää binääriä. VNT-kääntäjä litistää importatut moduulit nykyisessä buildissa yhdeksi käännösyksiköksi.

**Tee itse:** Kun siirrät funktion, etsi kaikki kutsujat ja lisää import oikeaan tiedostoon. Säilytä funktion nimi ja sopimus ensimmäisessä muutoksessa, jotta voit erottaa moduulirajan muutoksen toiminnallisesta muutoksesta.


## Toteutuslaboratorio: Modulaarinen kernel

Tässä osiossa teet konkreettisen muutoksen nykyiseen VNTOS-koodiin. Avaa lähdetiedostot ja varmista, että kuvaus vastaa nykyistä toteutusta.

**Tutkittavat tiedostot:** `os/kernel.vnt`, `os/kernel/core.vnt`, `os/kernel/shell.vnt`, `os/serial.vnt`

**Tavoite:** Siirrä yksi vastuu omaan VNT-moduuliin rikkomatta käynnistystä. Moduuliraja on sopimus, ei vain tiedoston siirto. Vältä import-syklejä ja piilotettua jaettua tilaa.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Valitse funktiot, jotka kuuluvat yhteen vastuualueeseen.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

2. **Etsi nykyinen toteutus:** Luo moduuli ja tuo vain sen todelliset riippuvuudet.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

3. **Kirjaa sopimus:** Tarkista julkiset funktiot ja nimet, joita muu kernel käyttää.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

4. **Tee pienin muutos:** Siirrä ensin toteutus muuttamatta käyttäytymistä.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

5. **Tarkista riippuvuudet:** Käännä ja varmista, että symbolit edelleen löytyvät.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

6. **Käännä:** Testaa sama käynnistyspolku ennen ja jälkeen muutoksen.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

7. **Tarkista konekoodi:** Poista duplikaatit vasta kun uusi moduuli toimii.
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

[← Edellinen oppitunti](./18-next-steps.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./20-interrupts-and-the-idt.md)
