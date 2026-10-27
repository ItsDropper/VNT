# Keskeytykset, poikkeukset ja IDT

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Keskeytys siirtää suorituksen erityiseen käsittelijään. Laite voi ilmoittaa tapahtumasta keskeytyksellä; CPU:n havaitsema virhe voi synnyttää poikkeuksen. IDT eli Interrupt Descriptor Table kertoo, mihin käsittelijään x86-64-CPU ohjaa tietyn vektorin.

## Sanasto

Vektori on tapahtuman numero. Käsittelijä reagoi tapahtumaan. IDT-merkintä sisältää käsittelijän osoitteen ja asetuksia. CPU:n ja assemblyn tallentama rekisteritila pitää vastata paluukoodin odotuksia.

## Käytännön harjoitus

Avaa os/arch/x86_64/interrupts.S ja os/interrupts.vnt. Seuraa, miten vektori siirtyy matalan tason sisääntulosta VNT-käsittelijälle. Tarkista, missä rekisterit tallennetaan ja miten suoritus palaa keskeytettyyn koodiin.

## Yleinen virhe ja turvallisuus

Keskeytyskäsittelijä voi keskeyttää muun toiminnon keskeltä. Pitkät tulostukset, lukot ja muistinhallintakutsut vaativat erityistä huolellisuutta.

## Tarkista ymmärtäminen

Mikä ero on keskeytyksellä ja poikkeuksella? Mitä IDT tekee?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Keskeytyksen käsittelyssä CPU:n tila ja pinon sisältö ovat osa rajapintaa. Jos assembly ja VNT-käsittelijä odottavat eri rakennetta, kentät tulkitaan väärin. Seuraa sekä sisääntuloa että paluuta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Jaa kernel VNT-moduuleihin](19-split-the-kernel-into-vnt-modules.md) · [Seuraava oppitunti: Pidä assembly-rajapinta pienenä →](21-minimal-assembly-entry-stubs.md)

## Toteutuslaboratorio: Keskeytykset ja IDT

Tämä harjoitus edellyttää lähdekoodin lukemista ja toteutuksen todentamista.

**Tutkittavat tiedostot:** `os/arch/x86_64/interrupts.S`, `os/interrupts.vnt`

**Tavoite:** Toteuta vektorin kulku CPU:n sisääntulosta VNT-dispatcheriin ja takaisin. Virhekoodilliset ja ilman virhekoodia tulevat vektorit vaativat eri stubin. Pino, ABI ja EOI ovat toiminnan osa, eivät sivuseikkoja.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Vertaa ISR_ERROR- ja ISR_NOERR-makroja ja listaa niiden vektorierot.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

2. **Etsi nykyinen toteutus:** Tarkista yhteisen sisääntulon rekisterien tallennusjärjestys.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

3. **Kirjaa sopimus:** Varmista, että VNT-dispatcherin parametrit vastaavat assembly-kutsua.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

4. **Tee pienin muutos:** Tarkista IDT-porttien osoitteet ja vektorit.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

5. **Tarkista riippuvuudet:** Tarkista paluupolku: rekisterit palautetaan ja lopuksi suoritetaan iretq.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

6. **Käännä:** Testaa yksi tunnettu vektori QEMU:ssa ja tarkista diagnostiikka.
   - Kirjaa lähdekoodin kohta, joka todistaa havainnon.
   - Tarkista, että kutsuja ja kutsuttava käyttävät samaa sopimusta.

7. **Tarkista konekoodi:** Älä laita tulostusta tai muistiallokointia tavalliseen IRQ-käsittelijään.
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
