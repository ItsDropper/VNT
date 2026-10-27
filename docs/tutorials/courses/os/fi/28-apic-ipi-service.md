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


## Syvempi harjoitus: seuraa koodia vaiheittain

IPI:n testaamisessa lähetys ei riitä: kohteen käsittely pitää todentaa. Silti testin tulos koskee vain polkua, jonka testi suorittaa, eikä sitä pidä yleistää muihin alijärjestelmiin.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: ACPI, APIC ja usea CPU-ydin](27-acpi-apic-and-smp.md) · [Seuraava oppitunti: SMP-turvallisuus: synkronoi ennen jaettua muistia →](29-smp-synchronization-contract.md)
