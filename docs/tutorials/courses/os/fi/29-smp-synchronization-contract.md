# SMP-turvallisuus: synkronoi ennen jaettua muistia

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kun usea CPU muuttaa samaa tietoa, operaatiot voivat limittyä. Race condition eli kilpailutilanne syntyy, kun lopputulos riippuu ajoituksesta. Esimerkiksi kaksi CPU:ta voi nähdä saman sivukehyksen vapaana ja varata sen molemmat.

## Sanasto

Kriittinen osio käsittelee jaettua tilaa, eikä vastaavaa osiota saa suorittaa samanaikaisesti. Lukko antaa yhdelle CPU:lle kerrallaan pääsyn. Atominen operaatio näkyy määritellyllä tavalla yhtenä tapahtumana myös muille CPU:ille.

## Käytännön harjoitus

Tutki os/mm/phys.vnt ja merkitse kohdat, joissa bittikartta ja laskuri muuttuvat. Kuvittele kahden CPU:n suorittavan saman varauksen yhtä aikaa. Kirjoita aikajana siitä, miten molemmat voivat lukea saman vapaan bitin ennen muutosta.

## Yleinen virhe ja turvallisuus

volatile ei ole lukko eikä tee usean käskyn kokonaisuudesta atomista. Inline-assembly ilman kääntäjälle ilmoitettuja muistivaikutuksia ei takaa oikeaa järjestystä. VNTOSin IPI-testi ei tee varaajasta SMP-turvallista.

## Tarkista ymmärtäminen

Piirrä kahden CPU:n kilpailutilanne. Missä kohdassa lukko tai atominen operaatio estäisi virheen?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: APIC IPI -viestit](28-apic-ipi-service.md) · [Kurssi valmis — takaisin sisällysluetteloon](./README.md)
