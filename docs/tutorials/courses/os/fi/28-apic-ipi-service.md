# APIC IPI -viestit

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

IPI on keskeytys, jonka yksi CPU lähettää toiselle. VNTOSin testipolku lähettää kiinteän vektorin IPI-viestejä ja tarkistaa, että kohde-CPU käsittelee ne. Näin voidaan testata CPU-ytimien välistä ilmoituspolkua.

## Sanasto

Lähettäjä lähettää viestin, kohde vastaanottaa sen ja vektori valitsee käsittelijän. Vastauslaskuri voi antaa testille näkyvän merkin käsittelijän suorituksesta.

## Käytännön harjoitus

Avaa os/kernel/smp.vnt ja os/arch/x86_64/apic.vnt. Etsi IPI:n lähetys, kohteen keskeytyskäsittelijä ja vastauslaskuri. Vertaa testin odotettua ja havaittua tulosta.

## Yleinen virhe ja turvallisuus

IPI-testi osoittaa viestin toimituksen kyseisessä testissä. Se ei todista, että muistivaraaja tai kaikki jaettu tila olisi SMP-turvallista, eikä se ole tehtäväajastin.

## Tarkista ymmärtäminen

Mitä IPI-testi todistaa? Nimeä kaksi asiaa, joita se ei todista.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: ACPI, APIC ja usea CPU-ydin](27-acpi-apic-and-smp.md) · [Seuraava oppitunti: SMP-turvallisuus: synkronoi ennen jaettua muistia →](29-smp-synchronization-contract.md)
