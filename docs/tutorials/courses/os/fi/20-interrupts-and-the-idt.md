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
