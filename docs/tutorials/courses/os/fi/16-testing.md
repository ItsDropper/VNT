# Testaa toimintaa, älä vain käännöstä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Käännös tarkistaa, että koodi voidaan kääntää. Linkitys tarkistaa symbolit ja muistiasettelun. QEMU-käynnistys tarkistaa, pääseekö kernel käyntiin. Toimintatesti tarkistaa, tapahtuuko odotettu asia. CI ajaa tarkistuksia automaattisesti palvelimella.

## Sanasto

Ennakkoehto on testin tarvitsema lähtötila. Odotettu tulos määrittää onnistumisen. Regressiotesti estää tunnetun virheen palaamisen. CI tarkoittaa jatkuvan integraation automaattisia tarkistuksia.

## Käytännön harjoitus

Määritä ennen muutosta testin syöte, odotettu tulos ja se, miten epäonnistuminen havaitaan. Keskeytystestissä käynnistyminen ei riitä; tarvitaan merkki käsittelijän suorituksesta. Muistitestissä pitää tarkistaa, ettei kehystä varata kahdesti.

## Yleinen virhe ja turvallisuus

Testi, joka vain tulostaa OK tarkistamatta väitettä, ei todista toimintaa. Älä väitä CI:tä onnistuneeksi, jos ajo on vielä kesken tai epäonnistunut.

## Tarkista ymmärtäminen

Valitse ominaisuus ja määritä ennakkoehto, toiminto, odotettu tulos ja epäonnistumisen tunnistus.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Hyvä testi epäonnistuu, kun ominaisuus on rikki. Mieti negatiivista tapausta: laite ei vastaa, syöte on väärä tai vapaa muisti loppuu. Myös epäonnistumisen pitää olla hallittu ja havaittava.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---


## Koodiesimerkki: testaa build, ELF ja käynnistys erikseen

```bash
bash os/build.sh
readelf -h -l build/kernel.elf
if command -v grub-file >/dev/null 2>&1; then
  grub-file --is-x86-multiboot2 build/kernel.elf
fi
if [ -f build/vntos.iso ]; then
  qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
else
  echo "ISO puuttuu: tarkista buildin tuloste ja ISO-työkalut."
fi
```

Komennot todistavat eri asioita: build-skripti yrittää luoda artefaktit, `readelf` näyttää ELF:n rakenteen, `grub-file` tarkistaa Multiboot2-yhteensopivuuden jos työkalu on asennettu ja QEMU testaa käynnistystä vain jos ISO syntyi.

**Tee itse:** Kirjaa jokaisen vaiheen poistumiskoodi ja ensimmäinen virhe. Älä kirjoita “kernel testattu”, jos tarkistit vain että ELF-tiedosto syntyi.

## Toteutuslaboratorio: Testaa muutakin kuin käännös

Tässä osiossa teet konkreettisen muutoksen nykyiseen VNTOS-koodiin. Avaa lähdetiedostot ja varmista, että kuvaus vastaa nykyistä toteutusta.

**Tutkittavat tiedostot:** `os/build.sh`, `.github/workflows/vntos-boot.yml`

**Tavoite:** Rakenna testimatriisi, joka erottaa käännöksen, bootin ja ominaisuuden. Jokaisella väitteellä pitää olla vastaava havainto. Testaa virheelliset syötteet ja rajat yhtä huolellisesti kuin normaali tapaus.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Kirjaa mitä build, grub-file, QEMU ja sarjaporttiloki kukin todistavat.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

2. **Etsi nykyinen toteutus:** Tee pieni testi, joka tarkistaa yhden toiminnon yhden ominaisuuden.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

3. **Kirjaa sopimus:** Lisää rajatapaus ja odotettu virhekäyttäytyminen.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

4. **Tee pienin muutos:** Tarkista, ettei testin onnistuminen perustu vain prosessin käynnistymiseen.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

5. **Tarkista riippuvuudet:** Erota ympäristöstä puuttuva työkalu ohjelmavirheestä.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

6. **Käännä:** Älä väitä CI:n läpäisseen, ellei GitHub Actionsin tulos todista sitä.
   - Kirjoita ylös havainto tai lähdekoodikohta, jolla varmistat tämän.
   - Älä tee seuraavaa muutosta ennen kuin tiedät nykyisen vaiheen tuloksen.

7. **Tarkista konekoodi:** Tallenna komento, tulos ja epäonnistumisen ensimmäinen rivi.
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

[← Edellinen oppitunti](./15-debugging.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./17-first-kernel-change.md)
