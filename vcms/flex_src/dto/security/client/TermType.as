package dto.security.client
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.TermType")]
	public class TermType
	{
		public var typeId:int;
		public var typeName:String;
		public var descn:String;
		public var termInfos:ArrayCollection;
		
		public function TermType()
		{
		}

	}
}