# Rakenna ja testaa keskeytyskernel

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Keskeytyksiä käyttävä kernel voi kääntyä mutta kaatua, kun CPU käyttää IDT:tä. Siksi build, käynnistys ja tapahtuman todentaminen ovat eri tarkistuksia. Testin pitää näyttää, että keskeytys saapui ja käsittelijä teki odotetun asian.

## Sanasto

Odotettu tulos kertoo, mistä onnistuminen tunnistetaan. Loki tallentaa tapahtumia. CI-ajo suorittaa automaattisia tarkistuksia palvelimella.

## Käytännön harjoitus

Aja projektin juuresta bash os/build.sh, käynnistä ISO QEMUssa ja seuraa sarjaporttia. Etsi konkreettinen tulos, kuten muuttunut laskuri. Tarkista GitHub Actionsin ajo ja lue epäonnistuneen vaiheen loki.

## Yleinen virhe ja turvallisuus

Kernelin käynnistyminen ei todista, että keskeytys toimitettiin. Tarkista erikseen ohjaimen alustus, IDT-merkintä, keskeytyksen salliminen ja kuittaus.

## Tarkista ymmärtäminen

Mikä havainto todistaisi, että käsittelijä todella suoritettiin?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Testaa yksi taso kerrallaan: build, käynnistys, keskeytyksen saapuminen ja sen vaikutus. Jos yksi taso epäonnistuu, korjaa se ennen seuraavan tuloksen tulkitsemista.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: PIT, PIC ja ajastimen tikit](23-pit-pic-and-timer-ticks.md) · [Seuraava oppitunti: Fyysinen muisti ja sivukehykset →](26-physical-memory-and-page-frames.md)

## Koodiesimerkki: varmista assembly ja VNT yhdessä

```bash
bash os/build.sh
readelf -s build/kernel.elf | grep -E 'interrupt_dispatch|vntos_interrupts_init|isr32'
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

Build-skripti kokoaa os/arch/x86_64/interrupts.S:n erilliseksi interrupts.o-tiedostoksi ja linkittää sen kernel-objektin kanssa. Symbolien tarkistus auttaa havaitsemaan, puuttuuko linkityksestä VNT-dispatcher tai assemblyn aloitusfunktio. QEMU-testi tarkistaa, pääseekö kernel käynnistymään.

**Tee itse:** Kirjaa buildin tulos, symbolit ja QEMU:n sarjaporttiloki erillisinä havaintoina. Jos kernel pysähtyy keskeytyksen jälkeen, rajaa vika sisääntulokehykseen, dispatchiin, EOI:hin tai paluupolkuun sen sijaan, että muuttaisit kaikkea yhtä aikaa.


## Toteutuslaboratorio: Keskeytyskernelin build ja testaus

Tämä harjoitus edellyttää lähdekoodin lukemista ja toteutuksen todentamista.

**Tutkittavat tiedostot:** `os/build.sh`, `os/arch/x86_64/interrupts.S`, `os/interrupts.vnt`

**Tavoite:** Todista käännös, linkitys, boot ja keskeytyksen käsittely erikseen. Yhden kerroksen onnistuminen ei todista seuraavaa. Pidä virhetilanteiden testit virtuaalikoneessa.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Tarkista, että interrupts.o sisältyy linkitykseen.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

2. **Etsi nykyinen toteutus:** Tarkista, että VNT:n dispatcher-symboli löytyy linkitetystä ELF:stä.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

3. **Kirjaa sopimus:** Varmista, että IDT on alustettu ennen keskeytysten avaamista.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

4. **Tee pienin muutos:** Käynnistä QEMU ja tarkista boot-tunniste ennen keskeytystestiä.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

5. **Tarkista riippuvuudet:** Tallenna testin vektori, odotettu tulos ja toteutunut sarjaporttiloki.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

6. **Käännä:** Jos testi kaatuu, rajaa ensin onko vika käännöksessä, sisääntulossa, dispatchissa vai paluussa.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

### Rajapinta- ja turvallisuustarkistus

- Varmista, että vektori, pino, rekisterit ja paluuarvo ovat täsmälleen sovitussa muodossa.
- Tarkista kaikki rajat ennen kuin luet muistia tai kirjoitat laitteistorekisteriin.
- Erota CPU:n vaatima assembly VNT:n politiikasta.
- Määritä virhetilanne: diagnoosi, hallittu pysäytys tai turvallinen ohitus.
- Tarkista, ettei uusi haara tee työtä, joka voi odottaa keskeytyksen jälkeen.
- Dokumentoi ominaisuudet, joita tämä muutos ei vielä toteuta.

### Todennus

1. Kirjoita odotettu vektori ja tulos ennen muutosta.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut build-vaihe.
4. Tarkista symbolit ja linkitetyt objektit, jos muutat ABI:a.
5. Käynnistä QEMU:ssa ja tallenna sarjaportin tuloste.
6. Testaa normaali vektori, rajavektori ja virhepolku.
7. Älä ilmoita testausta läpäistyksi ilman havaintoa.

### Tavalliset virheet

- Väärä stub-tyyppi siirtää pinon kentät.
- Rekisteri jää tallentamatta tai palautetaan väärässä järjestyksessä.
- EOI puuttuu tai tehdään kahdesti.
- IDT-portti osoittaa väärään symboliin.
- Keskeytyskäsittelijä jää odottamaan laitetta tai tekee pitkän tulostuksen.

### Muutosraportti

Kirjaa muutos, rajapintasopimus, build-tulos, QEMU-havainto, testatut vektorit ja avoimet rajoitukset.
