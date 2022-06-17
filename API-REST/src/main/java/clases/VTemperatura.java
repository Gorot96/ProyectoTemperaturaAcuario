package clases;

import java.util.Objects;

public class VTemperatura {
	private Integer idtemperatura;
	private Integer idSensor;
	private Double temperatura;
	private Long Mtemporal;

	public VTemperatura(Integer idtemperatura,Integer idSensor, Double temperatura, Long Mtemporal) {
		super();
		this.idtemperatura=idtemperatura;
		this.idSensor = idSensor;
		this.temperatura = temperatura;
		this.Mtemporal = Mtemporal;

	}
	public VTemperatura() {
		super();
	}
	
	

	public Integer getidtemperatura() {
		return idtemperatura;
	}
	public void setidtemperatura(Integer idtemperatura) {
		this.idtemperatura = idtemperatura;
	}
	public Integer getidSensor() {
		return idSensor;
	}

	public void setidSensor(Integer idSensor) {
		this.idSensor = idSensor;
	}

	public double getTemperatura() {
		return temperatura;
	}

	public void setTemperatura(double temperatura) {
		this.temperatura = temperatura;
	}

	public Long getMtemporal() {
		return Mtemporal;
	}

	public void setMtemporal(Long mtemporal) {
		Mtemporal = mtemporal;
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(Mtemporal, idtemperatura, idSensor, temperatura);
	}
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		VTemperatura other = (VTemperatura) obj;
		return Objects.equals(Mtemporal, other.Mtemporal) && Objects.equals(idtemperatura, other.idtemperatura)
				&& Objects.equals(idSensor, other.idSensor) && Objects.equals(temperatura, other.temperatura);
	}

	
	
	
	
	
}
