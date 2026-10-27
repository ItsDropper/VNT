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

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Jaa kernel VNT-moduuleihin](19-split-the-kernel-into-vnt-modules.md) · [Seuraava oppitunti: Pidä assembly-rajapinta pienenä →](21-minimal-assembly-entry-stubs.md)
