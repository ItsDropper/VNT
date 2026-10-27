# Freestanding-ohjelma

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Tavallinen sovellus voi pyytää käyttöjärjestelmältä tiedostoja, muistia ja verkkoyhteyksiä. Kernelin alkuvaiheessa näitä palveluja ei ole valmiina. Freestanding-koodi ei voi olettaa tavallisen standardikirjaston tai käyttöjärjestelmän runtime-ympäristön olevan käytössä.

## Sanasto

Hosted-ohjelma toimii käyttöjärjestelmän tarjoamassa ympäristössä. Runtime on suorituksen aikana tarvittavaa tukikoodia. ABI määrittää esimerkiksi argumenttien ja paluuarvojen välityksen.

## Käytännön harjoitus

Avaa src/compiler/freestanding_x86.c ja tutki freestanding-käsittelyä. Vertaa havaintoa os/kernel.vnt-tiedostoon. Kun löydät funktion, selvitä, käyttääkö se käyttöjärjestelmän palvelua tai ajonaikaista kirjastoa.

## Yleinen virhe ja turvallisuus

VNT-kielen syntaksin tuki ei tarkoita, että kaikki kirjastot toimivat kernelissä. Muistiallokointi tai tiedostokutsut vaativat erillisen toteutuksen, jos niitä tarvitaan.

## Tarkista ymmärtäminen

Miksi tavallisessa ohjelmassa toimiva funktio voi epäonnistua freestanding-kernelissä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Kun lisäät uuden funktion kernelin puolelle, tarkista, tarvitseeko se runtime-tukea. Tavallisessa ohjelmassa tuki voi tulla kirjastosta; kernelissä riippuvuus pitää tunnistaa ja ratkaista erikseen.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: ELF ja linkkeriskripti](11-elf-and-linker.md) · [Seuraava oppitunti: Sarjaportti debuggausvälineenä →](13-serial-io.md)

## Toteutuslaboratorio: Freestanding VNT

Tässä harjoituksessa toteutat tai jäljität toimivan osan, et vain kuvaile käsitettä.

**Tutkittavat tiedostot:** `os/kernel.vnt`, `os/build.sh`, `src/compiler/freestanding_x86.c`

**Tavoite:** Kirjoita VNT-funktio ilman käyttöjärjestelmän palveluja. Älä oleta libc:tä, tiedostoja tai dynaamista allokointia. Tarkista generoitu assembly, kun ABI tai unsafe muuttuu.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Lisää pieni funktio olemassa olevaan moduuliin ja tuo moduuli tarvittaessa.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

2. **Etsi nykyinen toteutus:** Tarkista, ettei funktio käytä linkittämätöntä ulkoista symbolia.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

3. **Kirjaa sopimus:** Käännä --emit-freestanding-asm-vaiheella ja etsi funktion symboli.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

4. **Tee pienin muutos:** Vertaa parametreja, paluuarvoa ja rekisterien käyttöä kutsujan odotukseen.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

5. **Tarkista riippuvuudet:** Testaa nolla, negatiivinen arvo ja yläraja.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

6. **Käännä:** Jos koodi ei käänny, tee pienin toistettava esimerkki ennen kääntäjän muuttamista.
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
